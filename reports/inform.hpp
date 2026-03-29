#pragma once
#include <vector>

// Genera el archivo PDF del informe de resultados.
// percVec  : porcentajes de cada partido
// quanVec  : cantidades absolutas de cada partido
// Devuelve true si el PDF se creó con exito, false si hubo error.
bool inform(std::vector<double> percVec, std::vector<int> quanVec);
