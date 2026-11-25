#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "pdfsolid_common_c.h"
#include "pdfsolid_conversion_c.h"

// A license key string, or the path of a license.xml file.
static const char* LICENSE = "../../../resource/license/license.xml";
static const char* PASSWORD = "";

static void progress_callback(int current_page_count, int total_page_count)
{
    printf("convert rate: %d/%d\n", current_page_count, total_page_count);
}

static bool cancel_callback(void)
{
    // Return true to cancel the conversion, false to continue.
    return false;
}

static void print_result(const char* name, CSDKErrorCode error_code)
{
    printf("%s: %d\n", name, (int)error_code);
}

static void run_word_to_pdf(CConvertCallback* callback)
{
    // Word to PDF reads only page_ranges and word_pdf_bookmark_mode; other fields are ignored.
    CConvertOption word_options = CPDF_CONVERT_OPTION_DEFAULT;
    char diagnostics[CPDF_WORD_TO_PDF_DIAGNOSTICS_SIZE] = { 0 };
    CSDKErrorCode error_code;

    // word to pdf with default options
    print_result("word to pdf",
                 CPDF_StartWordToPDF("../../input_files/word.docx",
                                     "../../output_files/word_to_pdf.pdf",
                                     word_options, callback, NULL));

    // word to pdf with heading bookmarks, a page range and diagnostics
    word_options.word_pdf_bookmark_mode = e_CWordPdfBookmarkHeadings;
    snprintf(word_options.page_ranges, sizeof(word_options.page_ranges), "%s", "1-2");
    error_code = CPDF_StartWordToPDF("../../input_files/word.docx",
                                     "../../output_files/word_to_pdf_bookmarks.pdf",
                                     word_options, callback, diagnostics);
    print_result("word to pdf with options", error_code);
    // e_CErrUnsupportedFeature means the PDF is usually still produced, but some
    // features may affect fidelity; the diagnostics text lists them.
    if (diagnostics[0] != '\0') {
        printf("word to pdf diagnostics:\n%s\n", diagnostics);
    }
}

int main(int argc, char** argv)
{
    CConvertOption options = CPDF_CONVERT_OPTION_DEFAULT;
    CConvertCallback callback;
    COCRLanguage languages[] = { e_CENGLISH };
    char version[30] = { 0 };

    // Zero-initialize so the optional AI hooks (ocr, layout, table, ...) stay null.
    memset(&callback, 0, sizeof(callback));
    callback.progress = progress_callback;
    callback.cancel = cancel_callback;

    print_result("PDFSolid Conversion SDK license verify",
                 CPDF_LicenseVerify(LICENSE, "", ""));
    CPDF_Initialize("../../../resource");
    CPDF_SetLogger(false, false);
    print_result("set document ai model",
                 CPDF_SetDocumentAIModel("../../../resource/models/documentai.model", -1));

    print_result("pdf to word",
                 CPDF_StartPDFToWord("../../input_files/word.pdf", PASSWORD,
                                     "../../output_files/word.docx", options, &callback));
    print_result("pdf to excel",
                 CPDF_StartPDFToExcel("../../input_files/excel.pdf", PASSWORD,
                                      "../../output_files/excel.xlsx", options, &callback));
    print_result("pdf to ppt",
                 CPDF_StartPDFToPpt("../../input_files/powerpoint.pdf", PASSWORD,
                                    "../../output_files/powerpoint.pptx", options, &callback));

    if (!(argc == 2 && strcmp(argv[1], "--office") == 0)) {
        options.excel_csv_format = true;
        print_result("pdf to csv",
                     CPDF_StartPDFToExcel("../../input_files/excel.pdf", PASSWORD,
                                          "../../output_files/", options, &callback));
        options.excel_csv_format = false;

        options.page_layout_mode = e_PageLayoutModeBox;
        print_result("pdf to html",
                     CPDF_StartPDFToHtml("../../input_files/word.pdf", PASSWORD,
                                         "../../output_files/html.html", options, &callback));
        print_result("pdf to rtf",
                     CPDF_StartPDFToRtf("../../input_files/word.pdf", PASSWORD,
                                        "../../output_files/rtf.rtf", options, &callback));
        print_result("pdf to image",
                     CPDF_StartPDFToImage("../../input_files/word.pdf", PASSWORD,
                                          "../../output_files/", options, &callback));
        print_result("pdf to txt",
                     CPDF_StartPDFToTxt("../../input_files/word.pdf", PASSWORD,
                                        "../../output_files/txt.txt", options, &callback));
        print_result("pdf to json",
                     CPDF_StartPDFToJson("../../input_files/word.pdf", PASSWORD,
                                         "../../output_files/json.json", options, &callback));
        print_result("pdf to markdown",
                     CPDF_StartPDFToMarkdown("../../input_files/word.pdf", PASSWORD,
                                             "../../output_files/markdown.md", options, &callback));

        options.enable_ocr = true;
        options.transparent_text = true;
        options.languages = languages;
        options.language_count = 1;
        print_result("pdf to searchable pdf",
                     CPDF_StartPDFToSearchablePDF("../../input_files/word.pdf", PASSWORD,
                                                  "../../output_files/pdf.pdf", options, &callback));
        print_result("pdf to ofd",
                     CPDF_StartPDFToOfd("../../input_files/word.pdf", PASSWORD,
                                        "../../output_files/pdf.ofd", options, &callback));

    }

    run_word_to_pdf(&callback);

    CPDF_GetVersion(version);
    printf("PDFSolid Conversion SDK version: %s\n", version);

    CPDF_ReleaseDocumentAIModel();
    CPDF_Release();
    return 0;
}
