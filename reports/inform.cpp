#include "../globals.hpp"
#include "inform.hpp"
#include <format>
#include <string>
#include <vector>

// ── HPDF error handler ────────────────────────────────────────────────────────
#ifdef HPDF_DLL
void __stdcall
#else
void
#endif
error_handler(HPDF_STATUS error_no, HPDF_STATUS detail_no, void* user_data)
{
    (void)user_data;
    printf("ERROR: error_no=%04X, detail_no=%u\n",
           (HPDF_UINT)error_no, (HPDF_UINT)detail_no);
    pdfRandomError  = true;
    pdfErrorString  = "ERROR: error_no="s +
                      std::format("{:04X}", (HPDF_UINT)error_no) +
                      " detail_no="s +
                      std::format("{}", (HPDF_UINT)detail_no);
    longjmp(env, 1);
}

// ── PDF report generation ─────────────────────────────────────────────────────
int inform(std::vector<double> percVec, std::vector<int> quanVec)
{
    HPDF_Doc   pdf;
    HPDF_Font  pdffont;
    HPDF_Page  p1;
    HPDF_Point pos;
    std::string txt  = "";
    std::vector<float> clr = {1.0f, 0.0f, 0.0f};
    float tw, last = 0;
    char* outPdfName = *informeName;

    pdf = HPDF_New(error_handler, NULL);
    if (!pdf) { pdfError = true; return 0; }
    if (setjmp(env)) { HPDF_Free(pdf); return 0; }

    const char* pdfFontName = HPDF_LoadTTFontFromFile(pdf, *pathPdfFont, HPDF_TRUE);
    pdffont = HPDF_GetFont(pdf, pdfFontName, NULL);
    if (!pdffont) { pdfFontError = true; return 0; }

    HPDF_SetCompressionMode(pdf, HPDF_COMP_ALL);
    HPDF_SetPageMode(pdf, HPDF_PAGE_MODE_USE_OUTLINE);
    p1 = HPDF_AddPage(pdf);
    HPDF_Page_BeginText(p1);
    HPDF_Page_SetRGBFill(p1, 0, 0, 0);
    HPDF_Page_SetTextRenderingMode(p1, HPDF_FILL);
    HPDF_Page_SetFontAndSize(p1, pdffont, 10);

    for (int p = 0; p < (int)percVec.size(); p++)
    {
        txt = partidosVec[p]->name;
        while ((int)strlen(txt.data()) < 7) txt += " ";
        txt += "| "s + std::to_string(quanVec[p]);
        txt += " ("s + std::to_string(percVec[p]);
        int rm = 4; while (rm > 0) { txt.pop_back(); rm--; }
        txt += "%)";
        tw = HPDF_Page_TextWidth(p1, txt.data());
        HPDF_Page_TextOut(p1,
                          (HPDF_Page_GetWidth(p1)) * 0.4f,
                          (HPDF_Page_GetHeight(p1) * 0.5f) - (10 + (p * 10)),
                          txt.data());
    }
    HPDF_Page_EndText(p1);

    for (int p = 0; p < (int)percVec.size(); p++)
    {
        HPDF_Page_SetRGBFill(p1, clr[0], clr[1], clr[2]);
        HPDF_Page_MoveTo(p1, 100, 400);
        if (p == 0) HPDF_Page_LineTo(p1, 100, 480);
        else        HPDF_Page_LineTo(p1, pos.x, pos.y);
        HPDF_Page_Arc(p1, 100, 400, 80,
                      360.0f * last,
                      360.0f * (last + (percVec[p] / 100.0f)));
        pos = HPDF_Page_GetCurrentPos(p1);
        HPDF_Page_LineTo(p1, 100, 400);
        HPDF_Page_Fill(p1);
        last += (percVec[p] / 100.0f);
        HPDF_Page_SetLineWidth(p1, 2);
        HPDF_Page_SetRGBStroke(p1, 0, 0, 0);
        HPDF_Page_SetRGBFill(p1, clr[0], clr[1], clr[2]);
        HPDF_Page_Rectangle(p1,
                             HPDF_Page_GetWidth(p1) * 0.388f,
                             (HPDF_Page_GetHeight(p1) * 0.501f) - (10 + (p * 10)),
                             5, 5);
        HPDF_Page_Fill(p1);

        if      (p < 2)  { clr.pop_back(); clr.insert(clr.begin(), 0.0f); }
        else if (p == 2) { clr.clear(); clr.push_back(1.0f); clr.push_back(1.0f); clr.push_back(0.0f); }
        else if (p == 3) { clr.clear(); clr.push_back(0.0f); clr.push_back(1.0f); clr.push_back(1.0f); }
        else if (p == 4) { clr.clear(); clr.push_back(1.0f); clr.push_back(0.0f); clr.push_back(1.0f); }
    }

    HPDF_SaveToFile(pdf, outPdfName);
    HPDF_Free(pdf);
    return 1;
}
