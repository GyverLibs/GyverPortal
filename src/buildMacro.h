#pragma once
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

#define GP_MAKE_FORM(act, subm, args) GP.FORM_BEGIN(act); args; GP.SUBMIT(subm); GP.FORM_END();

#define GP_MAKE_GRID(args) GP.GRID_BEGIN(); args; GP.GRID_END();

#define GP_MAKE_BOX(...) OVR_MACRO(GP_MAKE_BOX, __VA_ARGS__)
#define GP_MAKE_BOX1(args) GP.BOX_BEGIN(); args; GP.BOX_END();
#define GP_MAKE_BOX2(align, args) GP.BOX_BEGIN(align); args; GP.BOX_END();
#define GP_MAKE_BOX3(align, width, args) GP.BOX_BEGIN(align, width); args; GP.BOX_END();

#define GP_MAKE_BLOCK(...) OVR_MACRO(GP_MAKE_BLOCK, __VA_ARGS__)
#define GP_MAKE_BLOCK1(args) GP.BLOCK_BEGIN(); args; GP.BLOCK_END();
#define GP_MAKE_BLOCK2(width, args) GP.BLOCK_BEGIN(width); args; GP.BLOCK_END();

#define GP_MAKE_BLOCK_TAB(...) OVR_MACRO(GP_MAKE_BLOCK_TAB, __VA_ARGS__)
#define GP_MAKE_BLOCK_TAB2(text, args) GP.BLOCK_TAB_BEGIN(text); args; GP.BLOCK_END();
#define GP_MAKE_BLOCK_TAB3(text, width, args) GP.BLOCK_TAB_BEGIN(text, width); args; GP.BLOCK_END();
#define GP_MAKE_BLOCK_TAB4(text, width, style, args) GP.BLOCK_TAB_BEGIN(text, width, style); args; GP.BLOCK_END();

#define GP_MAKE_BLOCK_THIN(...) OVR_MACRO(GP_MAKE_BLOCK_THIN, __VA_ARGS__)
#define GP_MAKE_BLOCK_THIN1(args) GP.BLOCK_THIN_BEGIN(); args; GP.BLOCK_END();
#define GP_MAKE_BLOCK_THIN2(width, args) GP.BLOCK_THIN_BEGIN(width); args; GP.BLOCK_END();

#define GP_MAKE_BLOCK_THIN_TAB(...) OVR_MACRO(GP_MAKE_BLOCK_THIN_TAB, __VA_ARGS__)
#define GP_MAKE_BLOCK_THIN_TAB2(text, args) GP.BLOCK_THIN_TAB_BEGIN(text); args; GP.BLOCK_END();
#define GP_MAKE_BLOCK_THIN_TAB3(text, width, args) GP.BLOCK_THIN_TAB_BEGIN(text, width); args; GP.BLOCK_END();

#define GP_MAKE_JQ_UPDATE(...) OVR_MACRO(GP_MAKE_JQ_UPDATE, __VA_ARGS__)
#define GP_MAKE_JQ_UPDATE1(args) GP.JQ_UPDATE_BEGIN(); args; GP.JQ_UPDATE_END();
#define GP_MAKE_JQ_UPDATE2(prd, args) GP.JQ_UPDATE_BEGIN(prd); args; GP.JQ_UPDATE_END();
#define GP_MAKE_JQ_UPDATE3(prd, del, args) GP.JQ_UPDATE_BEGIN(prd, del); args; GP.JQ_UPDATE_END();

#define GP_MAKE_NAV_BLOCK(...) OVR_MACRO(GP_MAKE_NAV_BLOCK, __VA_ARGS__)
#define GP_MAKE_NAV_BLOCK1(args) GP.NAV_BLOCK_BEGIN(); args; GP.NAV_BLOCK_END();
#define GP_MAKE_NAV_BLOCK3(name, pos, args) GP.NAV_BLOCK_BEGIN(name, pos); args; GP.NAV_BLOCK_END();

#define GP_MAKE_SPOILER(...) OVR_MACRO(GP_MAKE_SPOILER, __VA_ARGS__)
#define GP_MAKE_SPOILER2(txt, args) GP.SPOILER_BEGIN(txt); args; GP.SPOILER_END();
#define GP_MAKE_SPOILER3(txt, style, args) GP.SPOILER_BEGIN(txt, style); args; GP.SPOILER_END();