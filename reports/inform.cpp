#include "../globals.hpp"
#include "inform.hpp"
#include <format>
#include <string>
#include <vector>

// ── HPDF error handler ────────────────────────────────────────────────────────
/* Esta función se encarga de manejar errores relacionados al PDF, lo que ocurre es que si la función inform() llega a tener un error en cualquier parte de la función después de asignar la variable "pdf" con HPDF_New, se manejará
   el error por medio de esta función error_handler() */
#ifdef HPDF_DLL       // Si la función error_handler() se ejecuta en windows...
void __stdcall        // declara a la función como void y llama a __stdcall, por que en windows se debe de empujar la función arriba del stack de la memoria, así que debe de llamarse __stdcall
#else                 // Si la función error_handler() NO se ejecuta en windows...
void                  // únicamente declarará a la función como void sin llamar a __stdcall
#endif
error_handler(HPDF_STATUS error_no,         // La función necesita el número de error que haya ocurrido
              HPDF_STATUS detail_no,        // La función necesita el número de detalle del error
              void* user_data)              // Y necesita a la variable user_data, esto por parámetros establecidos previamente, esta función la copié y pegue de libHaru, la librería encargada del PDF, entonces este argumento no sé extactamente su función
{
    pdfRandomError = true;                  // Si el programa llega a esta función, es por que ocurrió un error random muy probablemente, entonces declara a pdfRandomError como true
    if ((HPDF_UINT)error_no == 4182)        // El único código de error identificable es el 1056, que su valor UINT es 4182, este código de estado significa que...
    {
        pdfErrorString = "No hay partidos o votos para elaborar informe";       // ...faltan partidos o votos para elaborar el informe
    }
    else                                    // Si el error que ocurrió NO fue el 4182 en UINT, entonces procederá a mostrar el error específico en la pantalla
    {
        pdfErrorString  = "ERROR: error_no="s +                                 // El error a mostrar en pantalla sigue el siguiente formato: Error: error_no=numerodeerror detail_no=numerodedetalle
                          std::format("{:04X}", (HPDF_UINT)error_no) +
                          " detail_no="s +
                          std::format("{}", (HPDF_UINT)detail_no);
    }
    longjmp(env, 1);                        // Realiza un salto en la memoria a una posición en la que el error NO había ocurrido, evitando que el programa se explote, esa posición se define en inform()
}

// ── PDF report generation ─────────────────────────────────────────────────────
// Esta función se encarga de la generación del archivo PDF de informe, si el pdf se genera correctamente retornará true, en caso contrario retornará false
// La librería del PDF suele referirse en este programa como HPDF, pero si se desea investigar sobre funciones de la librería de mejor manera, el nombre es libHaru
bool inform(std::vector<double> percVec,         // Vector que contiene los porcentajes de los votos
           std::vector<int> quanVec)            // Vector que tiene las cantidades de los votos
{
    HPDF_Doc   pdf;                                             // Define a pdf como el documento del PDF
    HPDF_Font  pdffont;                                         // Define a pdffont como variable que almacenará el font del pdf
    HPDF_Page  p1;                                              // Define a p1 como una página del documento
    HPDF_Point pos;                                             // Define a pos como un punto específico en las ubicaciones del documento
    std::string txt  = "";                                      // txt es un string vacío, luego se llenará con los nombres de cada partido para el gráfico a generar en el PDF
    std::vector<float> clr = {1.0f, 0.0f, 0.0f};                // Es un vector que almacena el color actual a pintar un segmento del gráfico a generar en el PDF, en este caso, el color es rojo, más abajo se explican bien los colores
    float last = 0;                                             // last es el acumulador que almacena la última ubicación en el ángulo del gráfico del PDF
    char* outPdfName = *informeName;                            // outPdfName es el nombre del archivo de salida del PDF

    pdf = HPDF_New(error_handler, NULL);                        // Se asigna el valor de pdf, y en caso de que ocurra un error llamará a error_handler()
    if (!pdf) { pdfError = true; return false; }                // Si pdf tiene un error, activará a pdfError para indicar que hubo un error en la creación del PDF, y retornará false indicando que hubo un error
    if (setjmp(env)) { HPDF_Free(pdf); return false; }          // Define la posición actual en la memoria como lugar seguro a regresar en caso de que ocurra algún error, el cual se manejará con la función error_handler(), y retorna false

    const char* pdfFontName = HPDF_LoadTTFontFromFile(pdf, *pathPdfFont, HPDF_TRUE);        // Carga el nombre del font del PDF y lo guarda en pdfFontName
    pdffont = HPDF_GetFont(pdf, pdfFontName, NULL);                                         // pdffont almacena el font del PDF
    if (!pdffont) { pdfFontError = true; return false; }                                    // y si pdffont tuvo un error, activará a pdfFontError para indicar este error específico, y retorna false

    HPDF_SetCompressionMode(pdf, HPDF_COMP_ALL);                // Se define la compresión del PDF, en este caso se pasa como argumento HPDF_COMP_ALL, lo cual comprime absolutamente todo lo que contiene el PDF
    HPDF_SetPageMode(pdf, HPDF_PAGE_MODE_USE_NONE);             // Se define el modo del PDF, se pasa como argumento a HPDF_PAGE_MODE_USE_NONE para que no use ningun modo al abrir el PDF que no sea el predeterminado
    p1 = HPDF_AddPage(pdf);                                     // Se crea una página nueva, p1 almacena la página que se crea con la función HPDF_AddPage()

    // A partir de esta línea, se comienza la escritura del PDF

    HPDF_Page_BeginText(p1);                                    // Se inicia el bloque de código encargado de la escritura de texto
    HPDF_Page_SetRGBFill(p1, 0, 0, 0);                          // Se especifica el color del texto, para los colores se usa RGB, el 0,0,0 en RGB es el color negro
    HPDF_Page_SetTextRenderingMode(p1, HPDF_FILL);              // Se define el modo de escritura del texto, en este caso, se define que se rellenaran el contorno de las letras del texto, el cual es el modo normal
    HPDF_Page_SetFontAndSize(p1, pdffont, 10);                  // Se carga al texto la fuente definida previamente en pdffont y el tamaño de la fuente será de 10 pixeles

    for (int p = 0; p < (int)percVec.size(); p++)               // En este bucle recorrerá todos los partido
    {
        txt = partidosVec[p]->name;                               // txt guardará el nombre del partido actual que está recorriendo el bucle
        while ((int)strlen(txt.data()) < 7) txt += " ";           // Si el nombre del partido actual tiene menos de 7 carácteres, rellenará los carácteres faltantes con espacios, esto para que todos los partidos tengan el mismo largo del nombre
        txt += "| "s + std::to_string(quanVec[p]);                // para después, introducir un carácter pipe | y dar la apariencia de que es una tabla, además de que después del pipe se introduce el total de votos que tiene
        txt += " ("s + std::to_string(percVec[p]);                // y a parte del total de votos, también agrega entre paréntesis el porcentaje de votos
        int rm = 4; while (rm > 0) { txt.pop_back(); rm--; }      // y con el bucle while lo que hace es eliminar los últimos 4 valores del porcentaje, ya que el porcentaje tiene demasiados decimales, entonces los elimina
        txt += "%)";                                              // después de eliminar los decimales extra, agrega un símbolo de porciento % y finaliza con agregar un paréntesis final al string
        HPDF_Page_TextOut(p1,                                     // Posiciona el texto en la página actual
                          (HPDF_Page_GetWidth(p1)) * 0.4f,                            // En el 40% del ancho de la página
                          (HPDF_Page_GetHeight(p1) * 0.5f) - (10 + (p * 10)),         // Y en 50% del alto de la página, bajando 10 pixeles por el partido del bucle actual
                          txt.data());                                                // Luego, escribe el nombre del partido actual en el bucle en esa ubicación
    }
    HPDF_Page_EndText(p1);                                        // HPDF_Page_EndText() indica que es el final de la sección de la escritura de texto

    // A partir de esta línea, comienza el dibujado del gráfico del PDF

    for (int p = 0; p < (int)percVec.size(); p++)               // Este bucle recorrerá todos los partidos
    {

        // Dibujado de las secciones del círculo

        HPDF_Page_SetRGBFill(p1, clr[0], clr[1], clr[2]);         // Se define el color de relleno de la sección actual del gráfico, este color se define por el vector clr, el cual es cambiante
        HPDF_Page_MoveTo(p1, 100, 400);                           // Se desplaza al punto (100, 480) de la página para empezar el dibujado
        if (p == 0) HPDF_Page_LineTo(p1, 100, 480);               // El primer partido (p == 0) comenzará a dibujar arriba del círculo del gráfico, es decir, al inicio de todo el círculo se empezará el dibujado
        else        HPDF_Page_LineTo(p1, pos.x, pos.y);           // En caso de que el partido actual del bucle NO sea el primero, entonces comenzará donde el último partido terminó, por medio de la variable pos
        HPDF_Page_Arc(p1, 100, 400, 80,                           // Dibujará un arco, que comenzará en las ubicaciones (100, 400), además, el radio del círculo serán 80 pixeles...
                      360.0f * last,                              // el ángulo inicial del arco serán 360 grados multiplicados por last...
                      360.0f * (last + (percVec[p] / 100.0f)));   // y el ángulo final, es decir, donde termina el ángulo, será 360 multiplicados por la suma entre last y el valor porcentual de la cantidad de votos del partido actual
        pos = HPDF_Page_GetCurrentPos(p1);                        // pos almacena el eje "x" y "y" la posición exacta de donde terminó la última seccion del gráfico para usarse para dibujar el siguiente segmento del siguiente partido
        HPDF_Page_LineTo(p1, 100, 400);                           // Luego, se desplaza de nuevo a la posición (100, 400) del gráfico para volver al centro del círculo y terminar el segmento dibujado
        HPDF_Page_Fill(p1);                                       // Ahora, rellena el segmento dibujado con el color previamente seleccionado
        last += (percVec[p] / 100.0f);                            // last almacena el grado del segmento recién dibujado

        // Dibujado del pequeño cuadro para identificar el color de cada sección de cada partido y relacionarla con su nombre, cantidad de votos y porcentaje de votos

        HPDF_Page_SetLineWidth(p1, 2);                            // Define el ancho de los bordes del cuadrado a dibujar
        HPDF_Page_SetRGBStroke(p1, 0, 0, 0);                      // Define que los bordes del cuadrado serán de color negro
        HPDF_Page_SetRGBFill(p1, clr[0], clr[1], clr[2]);         // Define que el relleno del cuadrado será igual a los valores RGB que almacena actualmente el vector clr
        HPDF_Page_Rectangle(p1,                                   // Procede a dibujar el cuadrado actual
                             HPDF_Page_GetWidth(p1) * 0.388f,                         // Lo dibuja justo antes del inicio del nombre del partido actual
                             (HPDF_Page_GetHeight(p1) * 0.501f) - (10 + (p * 10)),    // Lo dibuja justo a la mitad de la altura del texto actual
                             5, 5);
        HPDF_Page_Fill(p1);                                       // Colorea el cuadrado

        // Cambiado del vector clr, para que los colores cambien
        /* El sistema RGB funciona de la siguiente manera: {ROJO, VERDE, AZUL} | Si el primer valor tiene un 1, significa que el rojo está en su mayor tonalidad, si el segundo valor tiene un 1 significa que el verde está en su mayor 
           tonalidad, etc, si los colores en este PDF funcionan de esta manera, lo que se puede hacer es modificar las posiciones de los unos y ceros para tener distintos colores, entonces...*/
        if      (p == 0) { clr.clear(); clr.push_back(0.0f); clr.push_back(1.0f); clr.push_back(0.0f); }    // Si el partido actual era el 0, ahora el color se modifica a verde
        else if (p == 1) { clr.clear(); clr.push_back(0.0f); clr.push_back(0.0f); clr.push_back(1.0f); }    // Ahora se modifica a azul
        else if (p == 2) { clr.clear(); clr.push_back(1.0f); clr.push_back(1.0f); clr.push_back(0.0f); }    // Ahora se modifica a amarillo
        else if (p == 3) { clr.clear(); clr.push_back(0.0f); clr.push_back(1.0f); clr.push_back(1.0f); }    // Ahora se modifica a cyan
        else if (p == 4) { clr.clear(); clr.push_back(1.0f); clr.push_back(0.0f); clr.push_back(1.0f); }    // Ahora se modifica a fucsia
        else if (p == 5) { clr.clear(); clr.push_back(1.0f); clr.push_back(0.5f); clr.push_back(1.0f); }    // Ahora se modifica a naranja
    }

    HPDF_SaveToFile(pdf, outPdfName);         // Guarda el PDF en el archivo de salida previamente definido por outPdfName
    HPDF_Free(pdf);                           // Libera la memoria de libHaru
    return true;    // Retorna true, indicando que todo ocurrió perfectamente
}
