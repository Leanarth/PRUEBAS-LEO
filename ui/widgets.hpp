#pragma once
#include <string>
#include "raylib.h"

// Clase padre — nombre derivado de las iniciales de sus propiedades: name, x, y, xsize, ysize, status
class nxyxys {
  public:
    std::string name;
    double xloc;
    double yloc;
    double xsize;
    double ysize;
    int status;
    Color highColor;
    Color normalColor;
    virtual void vincular() const = 0;
    virtual ~nxyxys() {}
};

class button : public nxyxys {
  public:
    void vincular() const override {}
    std::string selfquery = "";
    std::string outLog    = "";
    std::string oType     = "button";
    Color highColor   = {239, 184, 16, 255};   // VOCADORADO
    Color normalColor = {0,   200, 100, 255};  // VOCAVERDE
};

class inputBar : public nxyxys {
  public:
    void vincular() const override {}
    std::string   input;
    std::u32string input32;
    std::string oType     = "inputBar";
    Color highColor   = WHITE;
    Color normalColor = WHITE;
};

class sqlobject : public nxyxys {
  public:
    void vincular() const override {}
    int maxlen;
    std::string type;
    int id;
    std::string oType     = "sqlobject";
    Color highColor   = {239, 184, 16, 255};   // VOCADORADO
    Color normalColor = {0,   200, 100, 255};  // VOCAVERDE
};

class column : public nxyxys {
  public:
    void vincular() const override {}
    std::string  fromTable;
    std::u32string input32;
    std::string  input;
    std::string  type;
    std::string  maxlen;
    Color highColor   = WHITE;
    Color normalColor = WHITE;
    int id;
    std::string oType = "column";
};
