#pragma once

// HTML Canvas API
// https://www.w3schools.com/tags/ref_canvas.asp
// https://processing.org/reference/

#include "utils.h"
extern String* _GPP;

GP_PGM(CV_BUTT, "'butt'");
GP_PGM(CV_ROUND, "'round'");
GP_PGM(CV_SQUARE, "'square'");
GP_PGM(CV_PROJECT, "'square'");

GP_PGM(CV_BEVEL, "'bevel'");
GP_PGM(CV_MITER, "'miter'");

GP_PGM(CV_START, "'start'");
GP_PGM(CV_END, "'end'");
GP_PGM(CV_CENTER, "'center'");
GP_PGM(CV_LEFT, "'left'");
GP_PGM(CV_RIGHT, "'right'");

GP_PGM(CV_ALPHABETIC, "'alphabetic'");
GP_PGM(CV_TOP, "'top'");
GP_PGM(CV_HANGING, "'hanging'");
GP_PGM(CV_MIDDLE, "'middle'");
GP_PGM(CV_IDEOGRAPHIC, "'ideographic'");
GP_PGM(CV_BOTTOM, "'bottom'");

GP_PGM(CV_SRC_OVER, "'source-over'");
GP_PGM(CV_SRC_ATOP, "'source-atop'");
GP_PGM(CV_SRC_IN, "'source-in'");
GP_PGM(CV_SRC_OUT, "'source-out'");
GP_PGM(CV_DST_OVER, "'destination-over'");
GP_PGM(CV_DST_ATOP, "'destination-atop'");
GP_PGM(CV_DST_IN, "'destination-in'");
GP_PGM(CV_DST_OUT, "'destination-out'");
GP_PGM(CV_LIGHTER, "'lighter'");
GP_PGM(CV_COPY, "'copy'");
GP_PGM(CV_XOR, "'xor'");

GP_PGM(TXT_TOP, "'top'");
GP_PGM(TXT_BOTTOM, "'bottom'");
GP_PGM(TXT_CENTER, "'middle'");
GP_PGM(TXT_BASELINE, "'alphabetic'");

enum GP_DrawMode {
    M_CORNER,
    M_CORNERS,
    M_CENTER,
    M_RADIUS,
};

struct GPcanvas {
    GPcanvas(int sz = 500) {
        if (_GPP) ps = _GPP;    // внутри билдера
        else {
            s.reserve(sz);
            ps = &s;            // в программе
        }
    }
    
    // добавить строку кода на js (оканчивается; !!!)
    void add(const String& s) {
        _check();
        *ps += s;
    }
    
    // очистить буфер (для рисования снаружи билдера)
    void clearBuffer() {
        if (!_GPP) s = "";
    }
    
    // =====================================================
    // =============== PROCESSING-LIKE API =================
    // =====================================================
    
    // =================== BACKGROUND ======================
    // очистить полотно
    void clear() {
        clearRect(F("0,0,cv.width,cv.height"));
        beginPath();
    }
    
    // залить полотно цветом
    void background() {
        fillRect(F("0,0,cv.width,cv.height"));
    }
    void background(const String& v) {
        fillStyle(v);
        background();
    }
    void background(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255) {
        fillStyle(r, g, b, a);
        background();
    }
    void background(uint32_t hex) {
        fillStyle(hex);
        background();
    }
    
    // ======================== FILL =======================
    // выбрать цвет заливки
    void fill(const String& v) {
        fillStyle(v);
        fillF = 1;
    }
    void fill(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255) {
        fillStyle(r, g, b, a);
        fillF = 1;
    }
    void fill(uint32_t hex) {
        fillStyle(hex);
        fillF = 1;
    }
    
    // отключить заливку
    void noFill() {
        fillF = 0;
    }
    
    // ===================== STROKE ====================
    // выбрать цвет обводки
    void stroke(const String& v) {
        strokeStyle(v);
        strokeF = 1;
    }
    void stroke(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255) {
        strokeStyle(r, g, b, a);
        strokeF = 1;
    }
    void stroke(uint32_t hex) {
        strokeStyle(hex);
        strokeF = 1;
    }
    
    // отключить обводку
    void noStroke() {
        strokeF = 0;
    }
    
    // толщина обводки
    void strokeWeight(int v) {
        lineWidth(v);
    }
    
    // соединение линий: CV_MITER (умолч), CV_BEVEL, CV_ROUND
    void strokeJoin(PGM_P v) {
        lineJoin(v);
    }
    
    // края линий: CV_PROJECT (умолч), CV_ROUND, CV_SQUARE
    void strokeCap(PGM_P v) {
        lineCap(v);
    }
    
    // ===================== PRIMITIVES ====================
    // окружность
    void circle(int x, int y, int r) {
        beginPath();
        switch (rMode) {
        case M_CORNER: arc(x + r, y + r, r); break;
        default: arc(x, y, r); break;
        }
        if (strokeF) stroke();
        if (fillF) fill();
    }
    
    // линия
    void line(int x1, int y1, int x2, int y2) {
        beginPath();
        moveTo(x1, y1);
        lineTo(x2, y2);
        stroke();
    }
    
    // точка
    void point(int x, int y) {
        beginPath();
        fillRect(x, y, 1, 1);
    }
    
    // четырёхугольник
    void quad(int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4) {
        beginPath();
        moveTo(x1, y1);
        lineTo(x2, y2);
        lineTo(x3, y3);
        lineTo(x4, y4);
        closePath();
        if (strokeF) stroke();
        if (fillF) fill();
    }
    
    // треугольник
    void triangle(int x1, int y1, int x2, int y2, int x3, int y3) {
        beginPath();
        moveTo(x1, y1);
        lineTo(x2, y2);
        lineTo(x3, y3);
        closePath();
        if (strokeF) stroke();
        if (fillF) fill();
    }
    
    // прямоугольник
    void rect(int x, int y, int w, int h) {
        beginPath();
        switch (rMode) {
        case M_CORNER: _rect(x, y, w, h); break;
        case M_CORNERS: _rect(x, y, w - x, h - y); break;
        case M_CENTER: _rect(x - w / 2, y - h / 2, w, h); break;
        case M_RADIUS: _rect(x - w, y - h, w * 2, h * 2); break;
        }
        if (strokeF) stroke();
        if (fillF) fill();
    }
    
    // квадрат
    void square(int x, int y, int w) {
        rect(x, y, w, w);
    }
    
    // режим окружности: M_CENTER (умолч), M_CORNER
    void ellipseMode(GP_DrawMode mode) {
        eMode = mode;
    }
    
    // режим прямоугольника: M_CORNER (умолч), M_CORNERS, M_CENTER, M_RADIUS
    void rectMode(GP_DrawMode mode) {
        rMode = mode;
    }
    
    // ======================= TEXT ========================
    // шрифт
    void textFont(const char* name) {
        fname = name;
        _font();
    }
    
    // размер шрифта
    void textSize(int size) {
        fsize = size;
        _font();
    }
    
    // вывести текст
    void text(const String& text, int x, int y, int w = 0) {
        if (strokeF) strokeText(text, x, y, w);
        if (fillF) fillText(text, x, y, w);
    }
    
    // выравнивание текста
    // CV_LEFT, CV_CENTER, CV_RIGHT    
    // TXT_TOP, TXT_BOTTOM, TXT_CENTER, TXT_BASELINE
    void textAlign(PGM_P h, PGM_P v) {
        textAlign(h);
        textBaseline(v);
    }
    
    
    // ======================================================
    // ================== HTML CANVAS API ===================
    // ======================================================
    
    // цвет заполнения: "red", "#abc1230", "rgb(10,30,255)"
    void fillStyle(const String& v) {
        addCmd(0);
        color(v);
        semi();
    }
    void fillStyle(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255) {
        addCmd(0);
        color(r, g, b, a);
        semi();
    }
    void fillStyle(uint32_t hex) {
        addCmd(0);
        color(hex);
        semi();
    }
    
    // цвет обводки: "red", "#abc1230", "rgb(10,30,255)"
    void strokeStyle(const String& v) {
        addCmd(1);
        color(v);
        semi();
    }
    void strokeStyle(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255) {
        addCmd(1);
        color(r, g, b, a);
        semi();
    }
    void strokeStyle(uint32_t hex) {
        addCmd(1);
        color(hex);
        semi();
    }
    
    // цвет тени: "red", "#abc1230", "rgb(10,30,255)"
    void shadowColor(const String& v) {
        addCmd(2);
        color(v);
        semi();
    }
    void shadowColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255) {
        addCmd(2);
        color(r, g, b, a);
        semi();
    }
    void shadowColor(uint32_t hex) {
        addCmd(2);
        color(hex);
        semi();
    }
    
    // размытость тени в px
    void shadowBlur(int v) {
        addCmd(3);
        add(v);
        semi();
    }
    
    // отступ тени в px
    void shadowOffsetX(int v) {
        addCmd(4);
        add(v);
        semi();
    }
    
    // отступ тени в px
    void shadowOffsetY(int v) {
        addCmd(5);
        add(v);
        semi();
    }
    
    // края линий: CV_BUTT (умолч), CV_ROUND, CV_SQUARE
    // https://www.w3schools.com/tags/canvas_linecap.asp
    void lineCap(PGM_P v) {
        addCmd(6);
        add(FPSTR(v));
        semi();
    }
    
    // соединение линий: CV_MITER (умолч), CV_BEVEL, CV_ROUND
    // https://www.w3schools.com/tags/canvas_linejoin.asp
    void lineJoin(PGM_P v) {
        addCmd(7);
        add(FPSTR(v));
        semi();
    }
    
    // ширина линий в px
    void lineWidth(int v) {
        addCmd(8);
        add(v);
        semi();
    }
    
    // длина соединения CV_MITER
    // https://www.w3schools.com/tags/canvas_miterlimit.asp
    void miterLimit(int v) {
        addCmd(9);
        add(v);
        semi();
    }
    
    // шрифт: "30px Arial"
    // https://www.w3schools.com/tags/canvas_font.asp
    void font(const String& v) {
        addCmd(10);
        quote();
        add(v);
        quote();
        semi();
    }
    
    // выравнивание текста: CV_START (умолч), CV_END, CV_CENTER, CV_LEFT, CV_RIGHT
    // https://www.w3schools.com/tags/canvas_textalign.asp
    void textAlign(PGM_P v) {
        addCmd(11);
        add(FPSTR(v));
        semi();
    }
    
    // позиция текста: CV_ALPHABETIC (умолч), CV_TOP, CV_HANGING, CV_MIDDLE, CV_IDEOGRAPHIC, CV_BOTTOM
    // https://www.w3schools.com/tags/canvas_textbaseline.asp
    void textBaseline(PGM_P v) {
        addCmd(12);
        add(FPSTR(v));
        semi();
    }
    
    // прозрачность рисовки, 0.0-1.0
    void globalAlpha(float v) {
        addCmd(13);
        add(v);
        semi();
    }
    
    // тип наложения графики: CV_SRC_OVER (умолч), CV_SRC_ATOP, CV_SRC_IN, CV_SRC_OUT, CV_DST_OVER, CV_DST_ATOP, CV_DST_IN, CV_DST_OUT, CV_LIGHTER, CV_COPY, CV_XOR
    // https://www.w3schools.com/tags/canvas_globalcompositeoperation.asp
    void globalCompositeOperation(PGM_P v) {
        addCmd(14);
        add(FPSTR(v));
        semi();
    }
    
    // прямоугольник
    void _rect(int x, int y, int w, int h) {
        addCmd(15);
        params(4, x, y, w, h);
        semi();
    }
    
    /*// прямоугольник, параметры строкой "1,2,3,4"
    void rect(const String& v) {
        addCmd(15);
        add(v);
        semi();
    }*/
    
    // закрашенный прямоугольник
    void fillRect(int x, int y, int w, int h) {
        addCmd(16);
        params(4, x, y, w, h);
        semi();
    }
    
    // закрашенный прямоугольник, параметры строкой "1,2,3,4"
    void fillRect(const String& v) {
        addCmd(16);
        add(v);
        semi();
    }
    
    // обведённый прямоугольник
    void strokeRect(int x, int y, int w, int h) {
        addCmd(17);
        params(4, x, y, w, h);
        semi();
    }
    
    // обведённый прямоугольник, параметры строкой "1,2,3,4"
    void strokeRect(const String& v) {
        addCmd(17);
        add(v);
        semi();
    }
    
    // очистить область
    void clearRect(int x, int y, int w, int h) {
        addCmd(18);
        params(4, x, y, w, h);
        semi();
    }
    
    // очистить область, параметры строкой "1,2,3,4"
    void clearRect(const String& v) {
        addCmd(18);
        add(v);
        semi();
    }
    
    // залить
    void fill() {
        addCmd(19);
        semi();
    }
    
    // обвести
    void stroke() {
        addCmd(20);
        semi();
    }
    
    // начать путь
    void beginPath() {
        addCmd(21);
        semi();
    }
    
    // переместить курсор
    void moveTo(int x, int y) {
        addCmd(22);
        params(2, x, y);
        semi();
    }
    
    // завершить путь (провести линию на начало)
    void closePath() {
        addCmd(23);
        semi();
    }
    
    // нарисовать линию от курсора
    void lineTo(int x, int y) {
        addCmd(24);
        params(2, x, y);
        semi();
    }
    
    // ограничить область рисования
    // https://www.w3schools.com/tags/canvas_clip.asp
    void clip() {
        addCmd(25);
        semi();
    }
    
    // провести кривую
    // https://www.w3schools.com/tags/canvas_quadraticcurveto.asp
    void quadraticCurveTo(int cpx, int cpy, int x, int y) {
        addCmd(26);
        params(4, cpx, cpy, x, y);
        semi();
    }
    
    // провести кривую Безье
    // https://www.w3schools.com/tags/canvas_beziercurveto.asp
    void bezierCurveTo(int cp1x, int cp1y, int cp2x, int cp2y, int x, int y) {
        addCmd(27);
        params(6, cp1x, cp1y, cp2x, cp2y, x, y);
        semi();
    }
    
    // провести дугу
    // https://www.w3schools.com/tags/canvas_arc.asp
    void arc(int x, int y, int r, int sa = 0, int ea = 360, bool ccw = 0) {
        addCmd(28);
        params(3, x, y, r);
        comma();
        add((float)sa * DEG_TO_RAD);
        comma();
        add((float)ea * DEG_TO_RAD);
        if (ccw) add(",true");
        semi();
    }
    
    // скруглить
    // https://www.w3schools.com/tags/canvas_arcto.asp
    void arcTo(int x1, int y1, int x2, int y2, int r) {
        addCmd(29);
        params(5, x1, y1, x2, y2, r);
        semi();
    }
    
    // масштабировать область рисования
    // https://www.w3schools.com/tags/canvas_scale.asp
    void scale(int sw, int sh) {
        addCmd(30);
        params(2, sw, sh);
        semi();
    }
    
    // вращать область рисования
    // https://www.w3schools.com/tags/canvas_rotate.asp
    void rotate(int v) {
        addCmd(31);
        add((float)v * DEG_TO_RAD);
        semi();
    }
    
    // перемещать область рисования
    // https://www.w3schools.com/tags/canvas_translate.asp
    void translate(int x, int y) {
        addCmd(32);
        params(2, x, y);
        semi();
    }
    
    // вывести закрашенный текст, опционально макс. длина
    void fillText(const String& text, int x, int y, int w = 0) {
        addCmd(33);
        quote();
        add(text);
        quote();
        comma();
        params(2, x, y);
        if (w) {
            comma();
            add(w);
        }
        semi();
    }
    
    // вывести обведённый текст, опционально макс. длина
    void strokeText(const String& text, int x, int y, int w = 0) {
        addCmd(34);
        quote();
        add(text);
        quote();
        comma();
        params(2, x, y);
        if (w) {
            comma();
            add(w);
        }
        semi();
    }
    
    // вывести картинку
    // https://www.w3schools.com/tags/canvas_drawimage.asp
    void drawImage(const String& img, int x, int y) {
        addCmd(35);
        add(img);
        comma();
        params(2, x, y);
        semi();
    }
    void drawImage(const String& img, int x, int y, int w, int h) {
        addCmd(35);
        add(img);
        comma();
        params(4, x, y, w, h);
        semi();
    }
    void drawImage(const String& img, int sx, int sy, int sw, int sh, int x, int y, int w, int h) {
        addCmd(35);
        add(img);
        comma();
        params(8, sx, sy, sw, sh, x, y, w, h);
        semi();
    }
    
    // сохранить конфигурацию полотна
    void save() {
        addCmd(36);
        semi();
    }
    
    // восстановить конфигурацию полотна
    void restore() {
        addCmd(37);
        semi();
    }
    
    // private
    void addCmd(int cmd) {
        add(cmd);
        add("::");
    }
    void semi() {
        add(';');
    }
    void quote() {
        add('\'');
    }
    void comma() {
        add(',');
    }
    void params(int num, ...) {
        va_list valist;
        va_start(valist, num);
        for (int i = 0; i < num; i++) {
            add(va_arg(valist, int));
            if (i < num - 1) comma();
        }
        va_end(valist);
    }
    void add(int v) {
        _check();
        *ps += v;
    }
    void add(char v) {
        _check();
        *ps += v;
    }
    void add(double v) {
        _check();
        *ps += v;
    }
    
    void color(const String& v) {
        quote();
        add(v);
        quote();
    }
    void color(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255) {
        add(F("'rgb"));
        if (a != 255) add('a');
        add('(');
        add(r);
        comma();
        add(g);
        comma();
        add(b);
        if (a != 255) {
            comma();
            add((float)a / 255.0);
        }
        add(F(")'"));
    }
    void color(uint32_t hex) {
        quote();
        add('#');
        for (uint8_t i = 0; i < 6; i++) {
            char p = ((uint32_t)hex >> (5 - i) * 4) & 0xF;
            p += (p > 9) ? 87 : 48;
            add(p);
        }
        quote();
    }
    void _font() {
        String f(fsize);
        f += F("px ");
        f += fname;
        font(f);
    }
    
    String& _read() {
        sent = 1;
        return s;
    }
    void _check() {
        if (sent) {
            clearBuffer();
            sent = 0;
        }
    }
    
    String s;
    String* ps;
    bool strokeF = 1;
    bool fillF = 1;
    bool sent = 0;
    const char* fname = "Arial";
    int fsize = 20;
    GP_DrawMode eMode = M_RADIUS;
    GP_DrawMode rMode = M_CORNER;
};