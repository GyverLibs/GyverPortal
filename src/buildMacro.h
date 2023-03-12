#pragma once

// GP Macro

#include "builder.h"

// https://stackoverflow.com/a/30566098
#define OVR_MACRO(M, ...) _OVR(M, _COUNT_ARGS(__VA_ARGS__)) (__VA_ARGS__)
#define _OVR(mName, nArgs) _OVR_EXPAND(mName, nArgs)
#define _OVR_EXPAND(mName, nArgs) mName##nArgs
#define _COUNT_ARGS(...) _ARG_MATCH(__VA_ARGS__,9,8,7,6,5,4,3,2,1)
#define _ARG_MATCH(_1,_2,_3,_4,_5,_6,_7,_8,_9, N, ...) N

/*
// example
#define foo(...) OVR_MACRO(foo, __VA_ARGS__)
#define foo1(a) bar(a);
#define foo2(a, b) bar(a, b);
#define foo3(a, b, c) bar(a, b, c);
// ...
*/

#define GP_ALS(...) __extension__({static GPalign __als[] = {__VA_ARGS__}; __als;})

#define M_FORM(...) OVR_MACRO(M_FORM, __VA_ARGS__)
#define M_FORM2(act, args) GP.FORM_BEGIN(act); args; GP.FORM_END();
#define M_FORM3(act, subm, args) GP.FORM_BEGIN(act); args; GP.SUBMIT(subm); GP.FORM_END();

#define M_GRID(...) OVR_MACRO(M_GRID, __VA_ARGS__)
#define M_GRID1(args) GP.GRID_BEGIN(); args; GP.GRID_END();
#define M_GRID2(w, args) GP.GRID_BEGIN(w); args; GP.GRID_END();

#define M_TABLE(...) OVR_MACRO(M_TABLE, __VA_ARGS__)
#define M_TABLE1(args) GP.TABLE_BEGIN(); args; GP.TABLE_END();
#define M_TABLE2(tdw, args) GP.TABLE_BEGIN(tdw); args; GP.TABLE_END();
#define M_TABLE3(tdw, als, args) GP.TABLE_BEGIN(tdw, als); args; GP.TABLE_END();
#define M_TABLE4(tdw, als, w, args) GP.TABLE_BEGIN(tdw, als, w); args; GP.TABLE_END();

#define M_TD(...) OVR_MACRO(M_TD, __VA_ARGS__)
#define M_TD1(args) GP.TD(); args;
#define M_TD2(al, args) GP.TD(w, al); args;
#define M_TD3(al, cs, args) GP.TD(w, al, cs); args;
#define M_TD4(al, cs, rs, args) GP.TD(w, al, cs, rs); args;

#define M_TR(...) OVR_MACRO(M_TR, __VA_ARGS__)
#define M_TR1(a1) GP.TR(); GP.TD(); a1;
#define M_TR2(a1,a2) M_TR1(a1); GP.TD(); a2;
#define M_TR3(a1,a2,a3) M_TR2(a1,a2); GP.TD(); a3;
#define M_TR4(a1,a2,a3,a4) M_TR3(a1,a2,a3); GP.TD(); a4;
#define M_TR5(a1,a2,a3,a4,a5) M_TR4(a1,a2,a3,a4); GP.TD(); a5;
#define M_TR6(a1,a2,a3,a4,a5,a6) M_TR5(a1,a2,a3,a4,a5); GP.TD(); a6;
#define M_TR7(a1,a2,a3,a4,a5,a6,a7) M_TR6(a1,a2,a3,a4,a5,a6); GP.TD(); a7;
#define M_TR8(a1,a2,a3,a4,a5,a6,a7,a8) M_TR7(a1,a2,a3,a4,a5,a6,a7); GP.TD(); a8;
#define M_TR9(a1,a2,a3,a4,a5,a6,a7,a8,a9) M_TR8(a1,a2,a3,a4,a5,a6,a7,a8); GP.TD(); a9;
#define M_TR10(a1,a2,a3,a4,a5,a6,a7,a8,a9,a10) M_TR9(a1,a2,a3,a4,a5,a6,a7,a8,a9); GP.TD(); a10;

#define M_BOX(...) OVR_MACRO(M_BOX, __VA_ARGS__)
#define M_BOX1(args) GP.BOX_BEGIN(); args; GP.BOX_END();
#define M_BOX2(align, args) GP.BOX_BEGIN(align); args; GP.BOX_END();
#define M_BOX3(align, width, args) GP.BOX_BEGIN(align, width); args; GP.BOX_END();
#define M_BOX4(align, width, top, args) GP.BOX_BEGIN(align, width, top); args; GP.BOX_END();

#define M_BLOCK(...) OVR_MACRO(M_BLOCK, __VA_ARGS__)
#define M_BLOCK1(args) GP.BLOCK_BEGIN(); args; GP.BLOCK_END();
#define M_BLOCK2(type, args) GP.BLOCK_BEGIN(type); args; GP.BLOCK_END();
#define M_BLOCK3(type, width, args) GP.BLOCK_BEGIN(type, width); args; GP.BLOCK_END();
#define M_BLOCK4(type, width, text, args) GP.BLOCK_BEGIN(type, width, text); args; GP.BLOCK_END();
#define M_BLOCK5(type, width, text, style, args) GP.BLOCK_BEGIN(type, width, text, style); args; GP.BLOCK_END();

#define M_JQ_UPDATE(...) OVR_MACRO(M_JQ_UPDATE, __VA_ARGS__)
#define M_JQ_UPDATE1(args) GP.JQ_UPDATE_BEGIN(); args; GP.JQ_UPDATE_END();
#define M_JQ_UPDATE2(prd, args) GP.JQ_UPDATE_BEGIN(prd); args; GP.JQ_UPDATE_END();
#define M_JQ_UPDATE3(prd, del, args) GP.JQ_UPDATE_BEGIN(prd, del); args; GP.JQ_UPDATE_END();

#define M_NAV_BLOCK(...) OVR_MACRO(M_NAV_BLOCK, __VA_ARGS__)
#define M_NAV_BLOCK1(args) GP.NAV_BLOCK_BEGIN(); args; GP.NAV_BLOCK_END();
#define M_NAV_BLOCK3(name, pos, args) GP.NAV_BLOCK_BEGIN(name, pos); args; GP.NAV_BLOCK_END();

#define M_SPOILER(...) OVR_MACRO(M_SPOILER, __VA_ARGS__)
#define M_SPOILER2(txt, args) GP.SPOILER_BEGIN(txt); args; GP.SPOILER_END();
#define M_SPOILER3(txt, style, args) GP.SPOILER_BEGIN(txt, style); args; GP.SPOILER_END();


// legacy
#define M_BLOCK_TAB(...) OVR_MACRO(M_BLOCK_TAB, __VA_ARGS__)
#define M_BLOCK_TAB2(text, args) GP.BLOCK_TAB_BEGIN(text); args; GP.BLOCK_END();
#define M_BLOCK_TAB3(text, width, args) GP.BLOCK_TAB_BEGIN(text, width); args; GP.BLOCK_END();
#define M_BLOCK_TAB4(text, width, style, args) GP.BLOCK_TAB_BEGIN(text, width, style); args; GP.BLOCK_END();

#define M_BLOCK_THIN(...) OVR_MACRO(M_BLOCK_THIN, __VA_ARGS__)
#define M_BLOCK_THIN1(args) GP.BLOCK_THIN_BEGIN(); args; GP.BLOCK_END();
#define M_BLOCK_THIN2(width, args) GP.BLOCK_THIN_BEGIN(width); args; GP.BLOCK_END();

#define M_BLOCK_THIN_TAB(...) OVR_MACRO(M_BLOCK_THIN_TAB, __VA_ARGS__)
#define M_BLOCK_THIN_TAB2(text, args) GP.BLOCK_THIN_TAB_BEGIN(text); args; GP.BLOCK_END();
#define M_BLOCK_THIN_TAB3(text, width, args) GP.BLOCK_THIN_TAB_BEGIN(text, width); args; GP.BLOCK_END();


#define GP_MAKE_FORM(...) M_FORM(__VA_ARGS__)
#define GP_MAKE_GRID(...) M_GRID(__VA_ARGS__)
#define GP_MAKE_TABLE(...) M_TABLE(__VA_ARGS__)
#define GP_MAKE_TD(...) M_TD(__VA_ARGS__)
#define GP_MAKE_TR(...) M_TR(__VA_ARGS__)
#define GP_MAKE_BOX(...) M_BOX(__VA_ARGS__)
#define GP_MAKE_BLOCK(...) M_BLOCK(__VA_ARGS__)
#define GP_MAKE_BLOCK_TAB(...) M_BLOCK_TAB(__VA_ARGS__)
#define GP_MAKE_BLOCK_THIN(...) M_BLOCK_THIN(__VA_ARGS__)
#define GP_MAKE_BLOCK_THIN_TAB(...) M_BLOCK_THIN_TAB(__VA_ARGS__)
#define GP_MAKE_JQ_UPDATE(...) M_JQ_UPDATE(__VA_ARGS__)
#define GP_MAKE_NAV_BLOCK(...) M_NAV_BLOCK(__VA_ARGS__)
#define GP_MAKE_SPOILER(...) M_SPOILER(__VA_ARGS__)