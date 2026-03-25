#pragma once
#include <vector>

// Genera el archivo PDF del informe de resultados.
// percVec  : porcentajes de cada partido
// quanVec  : cantidades absolutas de cada partido
// Devuelve 1 si el PDF se creó con exito, 0 si hubo error.
int inform(std::vector<double> percVec, std::vector<int> quanVec);
