
/*  A Bison parser, made from BisonMe.y
 by  GNU Bison version 1.25
  */

#define YYBISON 1  /* Identify Bison output.  */

#define	T_ERROR	258
#define	T_INT	259
#define	T_FLOAT	260
#define	T_STRING	261
#define	TR_DEF	262
#define	TR_Separator	263
#define	TR_name	264
#define	TR_map	265
#define	TR_NONE	266
#define	TR_Info	267
#define	TR_string	268
#define	TR_PerspectiveCamera	269
#define	TR_OrthographicCamera	270
#define	TR_position	271
#define	TR_orientation	272
#define	TR_focalDistance	273
#define	TR_heightAngle	274
#define	TR_MaterialBinding	275
#define	TR_NormalBinding	276
#define	TR_value	277
#define	TR_OVERALL	278
#define	TR_PER_FACE	279
#define	TR_PER_FACE_INDEXED	280
#define	TR_PER_VERTEX	281
#define	TR_PER_VERTEX_INDEXED	282
#define	TR_ShapeHints	283
#define	TR_vertexOrdering	284
#define	TR_COUNTERCLOCKWISE	285
#define	TR_CLOCKWISE	286
#define	TR_shapeType	287
#define	TR_SOLID	288
#define	TR_UNKNOWN_SHAPE_TYPE	289
#define	TR_faceType	290
#define	TR_CONVEX	291
#define	TR_creaseAngle	292
#define	TR_MatrixTransform	293
#define	TR_matrix	294
#define	TR_Scale	295
#define	TR_scaleFactor	296
#define	TR_Rotation	297
#define	TR_rotation	298
#define	TR_Translation	299
#define	TR_translation	300
#define	TR_Texture2Transform	301
#define	TR_center	302
#define	TR_renderCulling	303
#define	TR_ON	304
#define	TR_OFF	305
#define	TR_AUTO	306
#define	TR_Texture2	307
#define	TR_filename	308
#define	TR_Coordinate3	309
#define	TR_point	310
#define	TR_Normal	311
#define	TR_vector	312
#define	TR_Material	313
#define	TR_ambientColor	314
#define	TR_specularColor	315
#define	TR_emissiveColor	316
#define	TR_diffuseColor	317
#define	TR_shininess	318
#define	TR_transparency	319
#define	TR_TextureCoordinate2	320
#define	TR_IndexedFaceSet	321
#define	TR_coordIndex	322
#define	TR_materialIndex	323
#define	TR_normalIndex	324
#define	TR_textureCoordIndex	325
#define	TR_Cylinder	326
#define	TR_Cube	327
#define	TR_radius	328
#define	TR_width	329
#define	TR_height	330
#define	TR_depth	331
#define	TR_DirectionalLight	332
#define	TR_PointLight	333
#define	TR_SpotLight	334
#define	TR_intensity	335
#define	TR_color	336
#define	TR_direction	337
#define	TR_location	338
#define	TR_on	339
#define	TR_dropOffRate	340
#define	TR_cutOffAngle	341
#define	TR_TRUE	342
#define	TR_FALSE	343
#define	TR_ScaleKey_ktx_com	344
#define	TR_scale	345
#define	TR_RotationKey_ktx_com	346
#define	TR_fields	347
#define	TR_frame	348
#define	TR_AnimationStyle_ktx_com	349
#define	TR_AnimationFrames_ktx_com	350
#define	TR_splineUse	351
#define	TR_pivotOffset	352
#define	TR_PositionKey_ktx_com	353
#define	TR_AimTarget_ktx_com	354
#define	TR_aimer	355
#define	TR_rate	356
#define	TR_length	357

#line 1 "BisonMe.y"

/*
 * BisonMe.y -- bison (yacc-like) parser description for VRML1.0.
 * Mostly by Bill Mark, 1998.
 * Enhanced by DaveMc, 1999.
 */

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <Model/LoadVRML.h>

int yylex();

/* define variables for return of actual semantic values from lexer
 * note that this is a kludge to avoid having to do a %union in bison,
 * which I discovered creates a large number of annoyances.
 */

/* make calls to yyerror produce verbose error messages (at least for now) */
#define YYDEBUG 1
#define YYERROR_VERBOSE 1

/* Prototypes */
char *tokentext(void);  /* return token text */


#line 31 "BisonMe.y"
typedef union {
  int		none; /* Used to flag values w/o type */
  int		ival;
  double	fval;
  char		*sval;
  double	*mptr;
  Vector	*vec;
  vector<int>	*ibuf;
  vector<double> *fbuf;
  vector<Vector> *vbuf;
} YYSTYPE;
#line 43 "BisonMe.y"

/* error */
#line 48 "BisonMe.y"

/* literals */
#line 55 "BisonMe.y"

/* Keywords.  All begin with TR_ */
#include <stdio.h>

#ifndef __cplusplus
#ifndef __STDC__
#define const
#endif
#endif



#define	YYFINAL		419
#define	YYFLAG		-32768
#define	YYNTBASE	108

#define YYTRANSLATE(x) ((unsigned)(x) <= 357 ? yytranslate[x] : 195)

static const char yytranslate[] = {     0,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,   107,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
   105,     2,   106,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,   103,     2,   104,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     1,     2,     3,     4,     5,
     6,     7,     8,     9,    10,    11,    12,    13,    14,    15,
    16,    17,    18,    19,    20,    21,    22,    23,    24,    25,
    26,    27,    28,    29,    30,    31,    32,    33,    34,    35,
    36,    37,    38,    39,    40,    41,    42,    43,    44,    45,
    46,    47,    48,    49,    50,    51,    52,    53,    54,    55,
    56,    57,    58,    59,    60,    61,    62,    63,    64,    65,
    66,    67,    68,    69,    70,    71,    72,    73,    74,    75,
    76,    77,    78,    79,    80,    81,    82,    83,    84,    85,
    86,    87,    88,    89,    90,    91,    92,    93,    94,    95,
    96,    97,    98,    99,   100,   101,   102
};

#if YYDEBUG != 0
static const short yyprhs[] = {     0,
     0,     2,     4,     8,     9,    16,    17,    23,    24,    30,
    35,    36,    42,    47,    52,    57,    62,    67,    72,    77,
    78,    84,    85,    91,    92,    98,   103,   108,   113,   114,
   120,   125,   126,   132,   137,   142,   147,   152,   157,   162,
   167,   172,   175,   176,   179,   180,   183,   186,   189,   191,
   193,   195,   198,   199,   202,   205,   206,   209,   213,   216,
   219,   222,   225,   226,   229,   232,   235,   238,   241,   244,
   247,   250,   251,   253,   255,   257,   259,   262,   265,   268,
   271,   274,   278,   281,   284,   287,   290,   293,   296,   299,
   302,   305,   308,   311,   314,   315,   318,   321,   324,   327,
   328,   331,   334,   337,   338,   343,   346,   348,   351,   352,
   357,   360,   363,   365,   368,   369,   374,   377,   379,   382,
   383,   388,   390,   393,   396,   397,   402,   405,   408,   411,
   412,   417,   420,   422,   426,   429,   432,   434,   436,   438,
   440,   442,   444,   446,   448,   450,   453,   454,   457,   460,
   463,   466,   469,   471,   473,   476,   493,   496,   497,   502,
   505,   510,   513,   518,   521,   523,   526,   531,   536,   541,
   546,   549,   550,   553,   556,   559,   562,   565,   568,   572,
   574,   578,   580,   584,   586,   590,   592,   596,   598,   602,
   604,   606,   609,   611,   615,   619,   623,   625,   628,   630,
   634,   637,   640,   642,   645,   646,   648,   652,   654,   656,
   659,   661,   665,   667,   669,   671,   673,   675
};

static const short yyrhs[] = {   109,
     0,   110,     0,     7,     6,   110,     0,     0,     8,   103,
   111,   121,   120,   104,     0,     0,    72,   103,   112,   141,
   104,     0,     0,    71,   103,   113,   143,   104,     0,    54,
   103,   166,   104,     0,     0,    77,   103,   114,   128,   104,
     0,    66,   103,   169,   104,     0,    12,   103,   124,   104,
     0,    58,   103,   171,   104,     0,    20,   103,   139,   104,
     0,    38,   103,   163,   104,     0,    56,   103,   167,   104,
     0,    21,   103,   140,   104,     0,     0,    15,   103,   115,
   126,   104,     0,     0,    14,   103,   116,   126,   104,     0,
     0,    78,   103,   117,   128,   104,     0,    42,   103,   137,
   104,     0,    40,   103,   136,   104,     0,    28,   103,   160,
   104,     0,     0,    79,   103,   118,   128,   104,     0,    52,
   103,   165,   104,     0,     0,    46,   103,   119,   130,   104,
     0,    65,   103,   168,   104,     0,    44,   103,   138,   104,
     0,    91,   103,   145,   104,     0,    89,   103,   147,   104,
     0,    98,   103,   149,   104,     0,    94,   103,   151,   104,
     0,    95,   103,   153,   104,     0,    99,   103,   155,   104,
     0,   120,   109,     0,     0,   121,   122,     0,     0,    48,
   123,     0,     9,     6,     0,    10,    11,     0,    49,     0,
    50,     0,    51,     0,   124,   125,     0,     0,    13,     6,
     0,   126,   127,     0,     0,    16,   182,     0,    17,   182,
   194,     0,    18,   194,     0,    19,   194,     0,    75,   194,
     0,   128,   129,     0,     0,    84,   193,     0,    80,   194,
     0,    81,   182,     0,    82,   182,     0,    83,   182,     0,
    85,   194,     0,    86,   194,     0,   130,   131,     0,     0,
   133,     0,   134,     0,   135,     0,   132,     0,    47,   186,
     0,    41,   186,     0,    43,   194,     0,    45,   186,     0,
    41,   182,     0,    43,   182,   194,     0,    45,   182,     0,
    22,    27,     0,    22,    25,     0,    22,    23,     0,    22,
    24,     0,    22,    26,     0,    22,    27,     0,    22,    25,
     0,    22,    23,     0,    22,    24,     0,    22,    26,     0,
   141,   142,     0,     0,    74,   194,     0,    75,   194,     0,
    76,   194,     0,   143,   144,     0,     0,    73,   194,     0,
    75,   194,     0,   145,   146,     0,     0,    92,   105,   157,
   106,     0,    93,   189,     0,   137,     0,   147,   148,     0,
     0,    92,   105,   157,   106,     0,    90,   182,     0,    93,
   189,     0,   137,     0,   149,   150,     0,     0,    92,   105,
   157,   106,     0,    93,   189,     0,   138,     0,   151,   152,
     0,     0,    92,   105,   157,   106,     0,    96,     0,    97,
   182,     0,   153,   154,     0,     0,    92,   105,   157,   106,
     0,   102,   189,     0,   101,   189,     0,   155,   156,     0,
     0,    92,   105,   157,   106,     0,   100,     6,     0,   158,
     0,   157,   107,   158,     0,     6,     6,     0,     6,   159,
     0,    43,     0,    45,     0,    97,     0,    96,     0,    93,
     0,   100,     0,    90,     0,   102,     0,   101,     0,   160,
   161,     0,     0,    29,    30,     0,    29,    31,     0,    32,
   162,     0,    35,    36,     0,    37,   194,     0,    33,     0,
    34,     0,    39,   164,     0,   194,   194,   194,   194,   194,
   194,   194,   194,   194,   194,   194,   194,   194,   194,   194,
   194,     0,    53,     6,     0,     0,    55,   105,   179,   106,
     0,    55,   181,     0,    57,   105,   179,   106,     0,    57,
   181,     0,    55,   105,   183,   106,     0,    55,   185,     0,
   170,     0,   169,   170,     0,    67,   105,   187,   106,     0,
    68,   105,   187,   106,     0,    69,   105,   187,   106,     0,
    70,   105,   187,   106,     0,   171,   172,     0,     0,    59,
   173,     0,    62,   174,     0,    61,   175,     0,    60,   176,
     0,    63,   177,     0,    64,   178,     0,   105,   179,   106,
     0,   181,     0,   105,   179,   106,     0,   181,     0,   105,
   179,   106,     0,   181,     0,   105,   179,   106,     0,   181,
     0,   105,   190,   106,     0,   192,     0,   105,   190,   106,
     0,   192,     0,   180,     0,   180,   107,     0,   182,     0,
   180,   107,   182,     0,   194,   194,   194,     0,   194,   194,
   194,     0,   184,     0,   184,   107,     0,   186,     0,   184,
   107,   186,     0,   194,   194,     0,   194,   194,     0,   188,
     0,   188,   107,     0,     0,   189,     0,   188,   107,   189,
     0,     4,     0,   191,     0,   191,   107,     0,   194,     0,
   191,   107,   194,     0,   194,     0,     4,     0,    87,     0,
    88,     0,     4,     0,     5,     0
};

#endif

#if YYDEBUG != 0
static const short yyrline[] = { 0,
   190,   193,   194,   197,   198,   199,   200,   201,   202,   203,
   204,   205,   206,   207,   208,   209,   210,   211,   212,   213,
   214,   215,   216,   217,   218,   219,   220,   221,   222,   223,
   224,   225,   226,   227,   228,   229,   230,   231,   232,   233,
   234,   237,   238,   241,   242,   245,   246,   247,   250,   251,
   252,   255,   256,   259,   262,   263,   266,   267,   268,   269,
   270,   273,   274,   277,   278,   279,   280,   281,   282,   283,
   286,   287,   290,   291,   292,   293,   296,   299,   302,   305,
   308,   311,   314,   317,   318,   319,   320,   321,   324,   325,
   326,   327,   328,   331,   332,   335,   336,   337,   340,   341,
   344,   345,   350,   351,   354,   355,   356,   359,   360,   363,
   364,   365,   366,   369,   370,   373,   374,   375,   378,   379,
   382,   383,   384,   387,   388,   391,   392,   393,   396,   397,
   400,   401,   404,   405,   407,   408,   411,   412,   413,   414,
   415,   416,   417,   418,   419,   424,   425,   428,   429,   430,
   431,   432,   435,   436,   439,   442,   454,   455,   458,   459,
   462,   463,   466,   467,   470,   471,   474,   475,   476,   477,
   480,   481,   484,   485,   486,   487,   488,   489,   492,   493,
   496,   497,   500,   501,   504,   505,   508,   509,   512,   513,
   516,   517,   520,   521,   524,   527,   530,   531,   534,   535,
   538,   541,   544,   545,   546,   549,   550,   553,   555,   556,
   559,   560,   563,   566,   567,   568,   571,   572
};
#endif


#if YYDEBUG != 0 || defined (YYERROR_VERBOSE)

static const char * const yytname[] = {   "$","error","$undefined.","T_ERROR",
"T_INT","T_FLOAT","T_STRING","TR_DEF","TR_Separator","TR_name","TR_map","TR_NONE",
"TR_Info","TR_string","TR_PerspectiveCamera","TR_OrthographicCamera","TR_position",
"TR_orientation","TR_focalDistance","TR_heightAngle","TR_MaterialBinding","TR_NormalBinding",
"TR_value","TR_OVERALL","TR_PER_FACE","TR_PER_FACE_INDEXED","TR_PER_VERTEX",
"TR_PER_VERTEX_INDEXED","TR_ShapeHints","TR_vertexOrdering","TR_COUNTERCLOCKWISE",
"TR_CLOCKWISE","TR_shapeType","TR_SOLID","TR_UNKNOWN_SHAPE_TYPE","TR_faceType",
"TR_CONVEX","TR_creaseAngle","TR_MatrixTransform","TR_matrix","TR_Scale","TR_scaleFactor",
"TR_Rotation","TR_rotation","TR_Translation","TR_translation","TR_Texture2Transform",
"TR_center","TR_renderCulling","TR_ON","TR_OFF","TR_AUTO","TR_Texture2","TR_filename",
"TR_Coordinate3","TR_point","TR_Normal","TR_vector","TR_Material","TR_ambientColor",
"TR_specularColor","TR_emissiveColor","TR_diffuseColor","TR_shininess","TR_transparency",
"TR_TextureCoordinate2","TR_IndexedFaceSet","TR_coordIndex","TR_materialIndex",
"TR_normalIndex","TR_textureCoordIndex","TR_Cylinder","TR_Cube","TR_radius",
"TR_width","TR_height","TR_depth","TR_DirectionalLight","TR_PointLight","TR_SpotLight",
"TR_intensity","TR_color","TR_direction","TR_location","TR_on","TR_dropOffRate",
"TR_cutOffAngle","TR_TRUE","TR_FALSE","TR_ScaleKey_ktx_com","TR_scale","TR_RotationKey_ktx_com",
"TR_fields","TR_frame","TR_AnimationStyle_ktx_com","TR_AnimationFrames_ktx_com",
"TR_splineUse","TR_pivotOffset","TR_PositionKey_ktx_com","TR_AimTarget_ktx_com",
"TR_aimer","TR_rate","TR_length","'{'","'}'","'['","']'","','","vrmlfile","node",
"rnode","@1","@2","@3","@4","@5","@6","@7","@8","@9","nodes","separator_fields",
"separator_field","rcopt","info_fields","info_field","cam_fields","cam_field",
"light_fields","light_field","t2t_fields","t2t_field","ce2_field","sc2_field",
"ro2_field","tr2_field","sc_field","ro_field","tr_field","mb_field","nb_field",
"cube_fields","cube_field","cylinder_fields","cylinder_field","rot_ktx_fields",
"rot_ktx_field","sca_ktx_fields","sca_ktx_field","pos_ktx_fields","pos_ktx_field",
"ani_ktx_fields","ani_ktx_field","anif_ktx_fields","anif_ktx_field","aim_ktx_fields",
"aim_ktx_field","ext_fields","ext_field","known_field_name","sh_fields","sh_field",
"shape_type","mt_field","matrix4x4","t2_field","c3_field","v3_field","tc2_field",
"ifs_fields","ifs_field","mat_fields","mat_field","acolor","dcolor","ecolor",
"scolor","shine","transp","triples","rtriples","onetriple","triple","doubles",
"rdoubles","onedouble","double","isingles","risingles","isingle","fsingles",
"rfsingles","onefsingle","sf_bool","fnum", NULL
};
#endif

static const short yyr1[] = {     0,
   108,   109,   109,   111,   110,   112,   110,   113,   110,   110,
   114,   110,   110,   110,   110,   110,   110,   110,   110,   115,
   110,   116,   110,   117,   110,   110,   110,   110,   118,   110,
   110,   119,   110,   110,   110,   110,   110,   110,   110,   110,
   110,   120,   120,   121,   121,   122,   122,   122,   123,   123,
   123,   124,   124,   125,   126,   126,   127,   127,   127,   127,
   127,   128,   128,   129,   129,   129,   129,   129,   129,   129,
   130,   130,   131,   131,   131,   131,   132,   133,   134,   135,
   136,   137,   138,   139,   139,   139,   139,   139,   140,   140,
   140,   140,   140,   141,   141,   142,   142,   142,   143,   143,
   144,   144,   145,   145,   146,   146,   146,   147,   147,   148,
   148,   148,   148,   149,   149,   150,   150,   150,   151,   151,
   152,   152,   152,   153,   153,   154,   154,   154,   155,   155,
   156,   156,   157,   157,   158,   158,   159,   159,   159,   159,
   159,   159,   159,   159,   159,   160,   160,   161,   161,   161,
   161,   161,   162,   162,   163,   164,   165,   165,   166,   166,
   167,   167,   168,   168,   169,   169,   170,   170,   170,   170,
   171,   171,   172,   172,   172,   172,   172,   172,   173,   173,
   174,   174,   175,   175,   176,   176,   177,   177,   178,   178,
   179,   179,   180,   180,   181,   182,   183,   183,   184,   184,
   185,   186,   187,   187,   187,   188,   188,   189,   190,   190,
   191,   191,   192,   193,   193,   193,   194,   194
};

static const short yyr2[] = {     0,
     1,     1,     3,     0,     6,     0,     5,     0,     5,     4,
     0,     5,     4,     4,     4,     4,     4,     4,     4,     0,
     5,     0,     5,     0,     5,     4,     4,     4,     0,     5,
     4,     0,     5,     4,     4,     4,     4,     4,     4,     4,
     4,     2,     0,     2,     0,     2,     2,     2,     1,     1,
     1,     2,     0,     2,     2,     0,     2,     3,     2,     2,
     2,     2,     0,     2,     2,     2,     2,     2,     2,     2,
     2,     0,     1,     1,     1,     1,     2,     2,     2,     2,
     2,     3,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     0,     2,     2,     2,     2,     0,
     2,     2,     2,     0,     4,     2,     1,     2,     0,     4,
     2,     2,     1,     2,     0,     4,     2,     1,     2,     0,
     4,     1,     2,     2,     0,     4,     2,     2,     2,     0,
     4,     2,     1,     3,     2,     2,     1,     1,     1,     1,
     1,     1,     1,     1,     1,     2,     0,     2,     2,     2,
     2,     2,     1,     1,     2,    16,     2,     0,     4,     2,
     4,     2,     4,     2,     1,     2,     4,     4,     4,     4,
     2,     0,     2,     2,     2,     2,     2,     2,     3,     1,
     3,     1,     3,     1,     3,     1,     3,     1,     3,     1,
     1,     2,     1,     3,     3,     3,     1,     2,     1,     3,
     2,     2,     1,     2,     0,     1,     3,     1,     1,     2,
     1,     3,     1,     1,     1,     1,     1,     1
};

static const short yydefact[] = {     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     1,     2,     0,     4,    53,    22,    20,     0,     0,   147,
     0,     0,     0,     0,    32,   158,     0,     0,   172,     0,
     0,     8,     6,    11,    24,    29,   109,   104,   120,   125,
   115,   130,     3,    45,     0,    56,    56,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
    72,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,   165,   100,    95,    63,    63,
    63,     0,     0,     0,     0,     0,     0,    43,     0,    14,
    52,     0,     0,    86,    87,    85,    88,    84,    16,    91,
    92,    90,    93,    89,    19,     0,     0,     0,     0,    28,
   146,   217,   218,   155,     0,    17,    81,     0,    27,     0,
    26,    83,    35,     0,   157,    31,     0,   160,     0,    10,
     0,   162,    18,     0,     0,     0,     0,     0,     0,    15,
   171,     0,   164,     0,    34,   205,   205,   205,   205,    13,
   166,     0,     0,     0,     0,     0,     0,     0,     0,    37,
   113,   108,     0,     0,    36,   107,   103,     0,   122,     0,
    39,   119,     0,     0,     0,    40,   124,     0,     0,    38,
   118,   114,     0,     0,    41,   129,     0,     0,     0,     0,
    44,    54,     0,     0,     0,     0,     0,    23,    55,    21,
   148,   149,   153,   154,   150,   151,   152,     0,     0,    82,
     0,     0,     0,     0,    33,    71,    76,    73,    74,    75,
     0,   191,   193,     0,     0,     0,   173,   180,     0,   176,
   186,     0,   175,   184,     0,   174,   182,     0,   177,   188,
   213,     0,   178,   190,     0,   197,   199,     0,   201,   208,
     0,   203,   206,     0,     0,     0,     0,     0,     9,    99,
     0,     0,     0,     7,    94,     0,     0,     0,     0,     0,
     0,     0,    12,    62,    25,    30,   111,     0,   112,     0,
   106,     0,   123,     0,   128,   127,     0,   117,     0,   132,
    47,    48,    49,    50,    51,    46,     5,    42,    57,     0,
    59,    60,    61,     0,   196,    78,    79,    80,    77,   159,
   192,   195,   161,     0,     0,     0,     0,     0,   209,   211,
     0,   163,   198,   202,   167,   204,   168,   169,   170,   101,
   102,    96,    97,    98,    65,    66,    67,    68,   214,   215,
   216,    64,    69,    70,     0,     0,   133,     0,     0,     0,
     0,     0,    58,     0,   194,   179,   185,   183,   181,   187,
   210,   189,   200,   207,   135,   137,   138,   143,   141,   140,
   139,   142,   145,   144,   136,   110,     0,   105,   121,   126,
   116,   131,     0,   212,   134,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,   156,     0,     0,     0
};

static const short yydefgoto[] = {   417,
    31,    32,    64,    98,    97,    99,    67,    66,   100,   101,
    81,   210,   108,   211,   316,    65,   111,   112,   219,   174,
   294,   144,   236,   237,   238,   239,   240,    76,    78,    80,
    69,    71,   173,   285,   172,   280,   103,   187,   102,   182,
   106,   202,   104,   192,   105,   197,   107,   206,   366,   367,
   395,    72,   131,   225,    74,   134,    83,    85,    87,    90,
    95,    96,    88,   161,   247,   256,   253,   250,   259,   263,
   241,   242,   148,   243,   265,   266,   163,   267,   271,   272,
   273,   338,   339,   260,   362,   138
};

static const short yypact[] = {   336,
    35,   -56,   -42,   -37,   -13,    -9,    11,    26,    28,    46,
    49,    65,    74,   114,   128,   135,   137,   143,   145,   147,
   157,   159,   163,   168,   169,   171,   175,   182,   185,   186,
-32768,-32768,   408,-32768,-32768,-32768,-32768,    76,    89,-32768,
    57,   116,   120,   125,-32768,   141,   170,   233,-32768,   236,
   227,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,    -7,-32768,-32768,    48,   198,   257,
   199,     8,    34,   200,    34,   201,    34,   202,    34,   203,
-32768,   303,   207,    10,   209,    15,   210,   139,    19,   213,
   216,   217,   218,   219,   123,-32768,-32768,-32768,-32768,-32768,
-32768,    58,    61,    84,   122,   -24,    -5,    54,   312,-32768,
-32768,    33,    41,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,    91,   133,   290,    34,-32768,
-32768,-32768,-32768,-32768,    34,-32768,-32768,    34,-32768,    34,
-32768,-32768,-32768,     1,-32768,-32768,    34,-32768,    34,-32768,
    34,-32768,-32768,    21,    23,    25,    27,    29,    31,-32768,
-32768,    34,-32768,    34,-32768,   323,   323,   323,   323,-32768,
-32768,    60,     3,   172,   285,   357,    34,   223,   323,-32768,
-32768,-32768,   224,   323,-32768,-32768,-32768,   226,-32768,    34,
-32768,-32768,   240,   323,   323,-32768,-32768,   241,   323,-32768,
-32768,-32768,   242,   326,-32768,-32768,   343,   341,     4,   221,
-32768,-32768,    34,    34,    34,    34,    34,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,    34,    34,-32768,
    34,    34,    34,    34,-32768,-32768,-32768,-32768,-32768,-32768,
   247,   248,-32768,    34,   252,    34,-32768,-32768,    34,-32768,
-32768,    34,-32768,-32768,    34,-32768,-32768,    34,-32768,-32768,
-32768,    34,-32768,-32768,   253,   254,-32768,    34,-32768,-32768,
   256,   265,-32768,   267,   269,   271,    34,    34,-32768,-32768,
    34,    34,    34,-32768,-32768,    34,    34,    34,    34,     5,
    34,    34,-32768,-32768,-32768,-32768,-32768,   348,-32768,   348,
-32768,   348,-32768,   348,-32768,-32768,   348,-32768,   348,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,    34,
-32768,-32768,-32768,    34,-32768,-32768,-32768,-32768,-32768,-32768,
    34,-32768,-32768,   273,   275,   277,   278,   279,   280,-32768,
   287,-32768,    34,-32768,-32768,   323,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,    82,    67,-32768,    80,    90,   100,
   105,   109,-32768,    34,-32768,-32768,-32768,-32768,-32768,-32768,
    34,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,   348,-32768,-32768,-32768,
-32768,-32768,    34,-32768,-32768,    34,    34,    34,    34,    34,
    34,    34,    34,    34,    34,-32768,   360,   363,-32768
};

static const short yypgoto[] = {-32768,
   176,   358,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,   328,-32768,   144,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,   166,   291,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,   -70,    -1,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,   304,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
  -146,-32768,   -16,   -67,-32768,-32768,-32768,  -230,   -50,-32768,
  -177,   136,-32768,   244,-32768,   -73
};


#define	YYLAST		507


static const short yytable[] = {   135,
   326,   299,   328,   329,   245,   109,   301,   137,   359,   140,
   149,   142,   149,   132,   133,   164,   305,   306,   132,   133,
    79,   308,   132,   133,   132,   133,   132,   133,   132,   133,
   132,   133,   132,   133,   132,   133,   126,   132,   133,   127,
    33,   231,   128,   232,   129,   233,    34,   234,   213,   214,
   215,   216,   313,   314,   315,   227,   213,   214,   215,   216,
    35,   228,   207,   208,   229,    36,   230,   198,   199,   152,
   114,   115,   116,   117,   118,   244,   281,   282,   283,   200,
   149,   149,   149,   149,   261,   261,   203,   385,   268,    37,
   269,   360,   361,    38,   204,    73,   110,    68,   205,   334,
    77,   209,   335,    77,   235,   336,   284,   217,   337,   297,
    70,   130,   383,    39,   147,   217,   274,   275,   276,   151,
   221,   222,   303,   162,   386,   246,   387,   249,    40,   252,
    41,   255,   277,   258,   278,   262,   218,   248,   251,   254,
   257,   321,   322,   323,   220,   319,   320,   177,    42,   178,
   179,    43,   183,   184,   324,   325,    75,   268,   327,   268,
   268,   180,    77,   279,   185,   223,   224,    44,   384,    79,
   332,   388,   396,   397,   389,   188,    45,   390,   391,   189,
   190,   392,   393,   394,   340,   398,   397,   191,   340,    91,
    92,    93,    94,    82,   344,   399,   397,   154,   155,   156,
   157,   158,   159,   350,   351,   400,   397,   352,   353,   354,
   401,   397,   355,   193,   402,   397,    46,   363,   364,   356,
   357,   358,   194,   195,    84,   196,   170,     1,     2,   368,
    47,   369,     3,   370,     4,     5,   371,    48,   372,    49,
     6,     7,   160,   175,   176,    50,   373,    51,     8,    52,
   374,   286,   287,   288,   289,   290,   291,   292,     9,    53,
    10,    54,    11,   375,    12,    55,    13,   181,   186,   268,
    56,    57,    14,    58,    15,   293,    16,    59,    17,   120,
   121,   122,   123,   124,    60,    18,    19,    61,    62,    86,
    89,    20,    21,    91,    92,    93,    94,    22,    23,    24,
   403,   119,   125,   136,   139,   141,   143,   404,   145,    25,
   146,    26,   150,   153,    27,    28,   165,   212,    29,    30,
   166,   167,   168,   169,   317,   226,   270,   298,   300,   406,
   302,   310,   407,   408,   409,   410,   411,   412,   413,   414,
   415,   416,     1,     2,   304,   307,   309,     3,   311,     4,
     5,   312,   330,   365,   331,     6,     7,   333,   342,   418,
   343,   345,   419,     8,   286,   287,   288,   289,   290,   291,
   292,   346,   347,     9,   348,    10,   349,    11,   376,    12,
   377,    13,   378,   379,   380,   318,   381,    14,   295,    15,
    63,    16,   382,    17,   113,   405,   201,   341,   171,     0,
    18,    19,   264,     0,     0,     0,    20,    21,     0,     0,
     0,     0,    22,    23,    24,     2,     0,     0,     0,     3,
     0,     4,     5,     0,    25,     0,    26,     6,     7,    27,
    28,     0,     0,    29,    30,     8,   286,   287,   288,   289,
   290,   291,   292,     0,     0,     9,     0,    10,     0,    11,
     0,    12,     0,    13,     0,     0,     0,     0,     0,    14,
   296,    15,     0,    16,     0,    17,     0,     0,     0,     0,
     0,     0,    18,    19,     0,     0,     0,     0,    20,    21,
     0,     0,     0,     0,    22,    23,    24,     0,     0,     0,
     0,     0,     0,     0,     0,     0,    25,     0,    26,     0,
     0,    27,    28,     0,     0,    29,    30
};

static const short yycheck[] = {    73,
   231,   179,   233,   234,   151,    13,   184,    75,     4,    77,
    84,    79,    86,     4,     5,    89,   194,   195,     4,     5,
    45,   199,     4,     5,     4,     5,     4,     5,     4,     5,
     4,     5,     4,     5,     4,     5,    29,     4,     5,    32,
     6,    41,    35,    43,    37,    45,   103,    47,    16,    17,
    18,    19,    49,    50,    51,   129,    16,    17,    18,    19,
   103,   135,     9,    10,   138,   103,   140,    92,    93,    86,
    23,    24,    25,    26,    27,   149,    74,    75,    76,   104,
   154,   155,   156,   157,   158,   159,    92,     6,   162,   103,
   164,    87,    88,   103,   100,    39,   104,    22,   104,   246,
    43,    48,   249,    43,   104,   252,   104,    75,   255,   177,
    22,   104,   343,   103,   105,    75,   167,   168,   169,   105,
    30,    31,   190,   105,    43,   105,    45,   105,   103,   105,
   103,   105,    73,   105,    75,   105,   104,   154,   155,   156,
   157,   215,   216,   217,   104,   213,   214,    90,   103,    92,
    93,   103,    92,    93,   228,   229,    41,   231,   232,   233,
   234,   104,    43,   104,   104,    33,    34,   103,   346,    45,
   244,    90,   106,   107,    93,    92,   103,    96,    97,    96,
    97,   100,   101,   102,   258,   106,   107,   104,   262,    67,
    68,    69,    70,    53,   268,   106,   107,    59,    60,    61,
    62,    63,    64,   277,   278,   106,   107,   281,   282,   283,
   106,   107,   286,    92,   106,   107,   103,   291,   292,   287,
   288,   289,   101,   102,    55,   104,   104,     7,     8,   300,
   103,   302,    12,   304,    14,    15,   307,   103,   309,   103,
    20,    21,   104,   100,   101,   103,   320,   103,    28,   103,
   324,    80,    81,    82,    83,    84,    85,    86,    38,   103,
    40,   103,    42,   331,    44,   103,    46,   102,   103,   343,
   103,   103,    52,   103,    54,   104,    56,   103,    58,    23,
    24,    25,    26,    27,   103,    65,    66,   103,   103,    57,
    55,    71,    72,    67,    68,    69,    70,    77,    78,    79,
   374,   104,   104,   104,   104,   104,   104,   381,     6,    89,
   104,    91,   104,   104,    94,    95,   104,     6,    98,    99,
   105,   105,   105,   105,   104,    36,     4,   105,   105,   403,
   105,     6,   406,   407,   408,   409,   410,   411,   412,   413,
   414,   415,     7,     8,   105,   105,   105,    12,     6,    14,
    15,    11,   106,     6,   107,    20,    21,   106,   106,     0,
   107,   106,     0,    28,    80,    81,    82,    83,    84,    85,
    86,   107,   106,    38,   106,    40,   106,    42,   106,    44,
   106,    46,   106,   106,   106,   210,   107,    52,   104,    54,
    33,    56,   106,    58,    67,   397,   106,   262,    95,    -1,
    65,    66,   159,    -1,    -1,    -1,    71,    72,    -1,    -1,
    -1,    -1,    77,    78,    79,     8,    -1,    -1,    -1,    12,
    -1,    14,    15,    -1,    89,    -1,    91,    20,    21,    94,
    95,    -1,    -1,    98,    99,    28,    80,    81,    82,    83,
    84,    85,    86,    -1,    -1,    38,    -1,    40,    -1,    42,
    -1,    44,    -1,    46,    -1,    -1,    -1,    -1,    -1,    52,
   104,    54,    -1,    56,    -1,    58,    -1,    -1,    -1,    -1,
    -1,    -1,    65,    66,    -1,    -1,    -1,    -1,    71,    72,
    -1,    -1,    -1,    -1,    77,    78,    79,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    89,    -1,    91,    -1,
    -1,    94,    95,    -1,    -1,    98,    99
};
/* -*-C-*-  Note some compilers choke on comments on `#line' lines.  */
#line 3 "bison.simple"

/* Skeleton output parser for bison,
   Copyright (C) 1984, 1989, 1990 Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.  */

/* As a special exception, when this file is copied by Bison into a
   Bison output file, you may use that output file without restriction.
   This special exception was added by the Free Software Foundation
   in version 1.24 of Bison.  */

#ifndef alloca
#ifdef __GNUC__
#define alloca __builtin_alloca
#else /* not GNU C.  */
#if (!defined (__STDC__) && defined (sparc)) || defined (__sparc__) || defined (__sparc) || defined (__sgi)
#include <alloca.h>
#else /* not sparc */
#if defined (MSDOS) && !defined (__TURBOC__)
#include <malloc.h>
#else /* not MSDOS, or __TURBOC__ */
#if defined(_AIX)
#include <malloc.h>
 #pragma alloca
#else /* not MSDOS, __TURBOC__, or _AIX */
#ifdef __hpux
#ifdef __cplusplus
extern "C" {
void *alloca (unsigned int);
};
#else /* not __cplusplus */
void *alloca ();
#endif /* not __cplusplus */
#endif /* __hpux */
#endif /* not _AIX */
#endif /* not MSDOS, or __TURBOC__ */
#endif /* not sparc.  */
#endif /* not GNU C.  */
#endif /* alloca not defined.  */

/* This is the parser code that is written into each bison parser
  when the %semantic_parser declaration is not specified in the grammar.
  It was written by Richard Stallman by simplifying the hairy parser
  used when %semantic_parser is specified.  */

/* Note: there must be only one dollar sign in this file.
   It is replaced by the list of actions, each action
   as one case of the switch.  */

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		-2
#define YYEOF		0
#define YYACCEPT	return(0)
#define YYABORT 	return(1)
#define YYERROR		goto yyerrlab1
/* Like YYERROR except do call yyerror.
   This remains here temporarily to ease the
   transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  */
#define YYFAIL		goto yyerrlab
#define YYRECOVERING()  (!!yyerrstatus)
#define YYBACKUP(token, value) \
do								\
  if (yychar == YYEMPTY && yylen == 1)				\
    { yychar = (token), yylval = (value);			\
      yychar1 = YYTRANSLATE (yychar);				\
      YYPOPSTACK;						\
      goto yybackup;						\
    }								\
  else								\
    { yyerror ("syntax error: cannot back up"); YYERROR; }	\
while (0)

#define YYTERROR	1
#define YYERRCODE	256

#ifndef YYPURE
#define YYLEX		yylex()
#endif

#ifdef YYPURE
#ifdef YYLSP_NEEDED
#ifdef YYLEX_PARAM
#define YYLEX		yylex(&yylval, &yylloc, YYLEX_PARAM)
#else
#define YYLEX		yylex(&yylval, &yylloc)
#endif
#else /* not YYLSP_NEEDED */
#ifdef YYLEX_PARAM
#define YYLEX		yylex(&yylval, YYLEX_PARAM)
#else
#define YYLEX		yylex(&yylval)
#endif
#endif /* not YYLSP_NEEDED */
#endif

/* If nonreentrant, generate the variables here */

#ifndef YYPURE

int	yychar;			/*  the lookahead symbol		*/
YYSTYPE	yylval;			/*  the semantic value of the		*/
				/*  lookahead symbol			*/

#ifdef YYLSP_NEEDED
YYLTYPE yylloc;			/*  location data for the lookahead	*/
				/*  symbol				*/
#endif

int yynerrs;			/*  number of parse errors so far       */
#endif  /* not YYPURE */

#if YYDEBUG != 0
int yydebug;			/*  nonzero means print parse trace	*/
/* Since this is uninitialized, it does not stop multiple parsers
   from coexisting.  */
#endif

/*  YYINITDEPTH indicates the initial size of the parser's stacks	*/

#ifndef	YYINITDEPTH
#define YYINITDEPTH 200
#endif

/*  YYMAXDEPTH is the maximum size the stacks can grow to
    (effective only if the built-in stack extension method is used).  */

#if YYMAXDEPTH == 0
#undef YYMAXDEPTH
#endif

#ifndef YYMAXDEPTH
#define YYMAXDEPTH 10000
#endif

/* Prevent warning if -Wstrict-prototypes.  */
#ifdef __GNUC__
int yyparse (void);
#endif

#if __GNUC__ > 1		/* GNU C and GNU C++ define this.  */
#define __yy_memcpy(TO,FROM,COUNT)	__builtin_memcpy(TO,FROM,COUNT)
#else				/* not GNU C or C++ */
#ifndef __cplusplus

/* This is the most reliable way to avoid incompatibilities
   in available built-in functions on various systems.  */
static void
__yy_memcpy (to, from, count)
     char *to;
     char *from;
     int count;
{
  register char *f = from;
  register char *t = to;
  register int i = count;

  while (i-- > 0)
    *t++ = *f++;
}

#else /* __cplusplus */

/* This is the most reliable way to avoid incompatibilities
   in available built-in functions on various systems.  */
static void
__yy_memcpy (char *to, char *from, int count)
{
  register char *f = from;
  register char *t = to;
  register int i = count;

  while (i-- > 0)
    *t++ = *f++;
}

#endif
#endif

#line 196 "bison.simple"

/* The user can define YYPARSE_PARAM as the name of an argument to be passed
   into yyparse.  The argument should have type void *.
   It should actually point to an object.
   Grammar actions can access the variable by casting it
   to the proper pointer type.  */

#ifdef YYPARSE_PARAM
#ifdef __cplusplus
#define YYPARSE_PARAM_ARG void *YYPARSE_PARAM
#define YYPARSE_PARAM_DECL
#else /* not __cplusplus */
#define YYPARSE_PARAM_ARG YYPARSE_PARAM
#define YYPARSE_PARAM_DECL void *YYPARSE_PARAM;
#endif /* not __cplusplus */
#else /* not YYPARSE_PARAM */
#define YYPARSE_PARAM_ARG
#define YYPARSE_PARAM_DECL
#endif /* not YYPARSE_PARAM */

int
yyparse(YYPARSE_PARAM_ARG)
     YYPARSE_PARAM_DECL
{
  register int yystate;
  register int yyn;
  register short *yyssp;
  register YYSTYPE *yyvsp;
  int yyerrstatus;	/*  number of tokens to shift before error messages enabled */
  int yychar1 = 0;		/*  lookahead token as an internal (translated) token number */

  short	yyssa[YYINITDEPTH];	/*  the state stack			*/
  YYSTYPE yyvsa[YYINITDEPTH];	/*  the semantic value stack		*/

  short *yyss = yyssa;		/*  refer to the stacks thru separate pointers */
  YYSTYPE *yyvs = yyvsa;	/*  to allow yyoverflow to reallocate them elsewhere */

#ifdef YYLSP_NEEDED
  YYLTYPE yylsa[YYINITDEPTH];	/*  the location stack			*/
  YYLTYPE *yyls = yylsa;
  YYLTYPE *yylsp;

#define YYPOPSTACK   (yyvsp--, yyssp--, yylsp--)
#else
#define YYPOPSTACK   (yyvsp--, yyssp--)
#endif

  int yystacksize = YYINITDEPTH;

#ifdef YYPURE
  int yychar;
  YYSTYPE yylval;
  int yynerrs;
#ifdef YYLSP_NEEDED
  YYLTYPE yylloc;
#endif
#endif

  YYSTYPE yyval;		/*  the variable used to return		*/
				/*  semantic values from the action	*/
				/*  routines				*/

  int yylen;

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Starting parse\n");
#endif

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY;		/* Cause a token to be read.  */

  /* Initialize stack pointers.
     Waste one element of value and location stack
     so that they stay on the same level as the state stack.
     The wasted elements are never initialized.  */

  yyssp = yyss - 1;
  yyvsp = yyvs;
#ifdef YYLSP_NEEDED
  yylsp = yyls;
#endif

/* Push a new state, which is found in  yystate  .  */
/* In all cases, when you get here, the value and location stacks
   have just been pushed. so pushing a state here evens the stacks.  */
yynewstate:

  *++yyssp = yystate;

  if (yyssp >= yyss + yystacksize - 1)
    {
      /* Give user a chance to reallocate the stack */
      /* Use copies of these so that the &'s don't force the real ones into memory. */
      YYSTYPE *yyvs1 = yyvs;
      short *yyss1 = yyss;
#ifdef YYLSP_NEEDED
      YYLTYPE *yyls1 = yyls;
#endif

      /* Get the current used size of the three stacks, in elements.  */
      int size = yyssp - yyss + 1;

#ifdef yyoverflow
      /* Each stack pointer address is followed by the size of
	 the data in use in that stack, in bytes.  */
#ifdef YYLSP_NEEDED
      /* This used to be a conditional around just the two extra args,
	 but that might be undefined if yyoverflow is a macro.  */
      yyoverflow("parser stack overflow",
		 &yyss1, size * sizeof (*yyssp),
		 &yyvs1, size * sizeof (*yyvsp),
		 &yyls1, size * sizeof (*yylsp),
		 &yystacksize);
#else
      yyoverflow("parser stack overflow",
		 &yyss1, size * sizeof (*yyssp),
		 &yyvs1, size * sizeof (*yyvsp),
		 &yystacksize);
#endif

      yyss = yyss1; yyvs = yyvs1;
#ifdef YYLSP_NEEDED
      yyls = yyls1;
#endif
#else /* no yyoverflow */
      /* Extend the stack our own way.  */
      if (yystacksize >= YYMAXDEPTH)
	{
	  yyerror("parser stack overflow");
	  return 2;
	}
      yystacksize *= 2;
      if (yystacksize > YYMAXDEPTH)
	yystacksize = YYMAXDEPTH;
      yyss = (short *) alloca (yystacksize * sizeof (*yyssp));
      __yy_memcpy ((char *)yyss, (char *)yyss1, size * sizeof (*yyssp));
      yyvs = (YYSTYPE *) alloca (yystacksize * sizeof (*yyvsp));
      __yy_memcpy ((char *)yyvs, (char *)yyvs1, size * sizeof (*yyvsp));
#ifdef YYLSP_NEEDED
      yyls = (YYLTYPE *) alloca (yystacksize * sizeof (*yylsp));
      __yy_memcpy ((char *)yyls, (char *)yyls1, size * sizeof (*yylsp));
#endif
#endif /* no yyoverflow */

      yyssp = yyss + size - 1;
      yyvsp = yyvs + size - 1;
#ifdef YYLSP_NEEDED
      yylsp = yyls + size - 1;
#endif

#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Stack size increased to %d\n", yystacksize);
#endif

      if (yyssp >= yyss + yystacksize - 1)
	YYABORT;
    }

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Entering state %d\n", yystate);
#endif

  goto yybackup;
 yybackup:

/* Do appropriate processing given the current state.  */
/* Read a lookahead token if we need one and don't already have one.  */
/* yyresume: */

  /* First try to decide what to do without reference to lookahead token.  */

  yyn = yypact[yystate];
  if (yyn == YYFLAG)
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* yychar is either YYEMPTY or YYEOF
     or a valid token in external form.  */

  if (yychar == YYEMPTY)
    {
#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Reading a token: ");
#endif
      yychar = YYLEX;
    }

  /* Convert token to internal form (in yychar1) for indexing tables with */

  if (yychar <= 0)		/* This means end of input. */
    {
      yychar1 = 0;
      yychar = YYEOF;		/* Don't call YYLEX any more */

#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Now at end of input.\n");
#endif
    }
  else
    {
      yychar1 = YYTRANSLATE(yychar);

#if YYDEBUG != 0
      if (yydebug)
	{
	  fprintf (stderr, "Next token is %d (%s", yychar, yytname[yychar1]);
	  /* Give the individual parser a way to print the precise meaning
	     of a token, for further debugging info.  */
#ifdef YYPRINT
	  YYPRINT (stderr, yychar, yylval);
#endif
	  fprintf (stderr, ")\n");
	}
#endif
    }

  yyn += yychar1;
  if (yyn < 0 || yyn > YYLAST || yycheck[yyn] != yychar1)
    goto yydefault;

  yyn = yytable[yyn];

  /* yyn is what to do for this token type in this state.
     Negative => reduce, -yyn is rule number.
     Positive => shift, yyn is new state.
       New state is final state => don't bother to shift,
       just return success.
     0, or most negative number => error.  */

  if (yyn < 0)
    {
      if (yyn == YYFLAG)
	goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }
  else if (yyn == 0)
    goto yyerrlab;

  if (yyn == YYFINAL)
    YYACCEPT;

  /* Shift the lookahead token.  */

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Shifting token %d (%s), ", yychar, yytname[yychar1]);
#endif

  /* Discard the token being shifted unless it is eof.  */
  if (yychar != YYEOF)
    yychar = YYEMPTY;

  *++yyvsp = yylval;
#ifdef YYLSP_NEEDED
  *++yylsp = yylloc;
#endif

  /* count tokens shifted since error; after three, turn off error status.  */
  if (yyerrstatus) yyerrstatus--;

  yystate = yyn;
  goto yynewstate;

/* Do the default action for the current state.  */
yydefault:

  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;

/* Do a reduction.  yyn is the number of a rule to reduce with.  */
yyreduce:
  yylen = yyr2[yyn];
  if (yylen > 0)
    yyval = yyvsp[1-yylen]; /* implement default value of the action */

#if YYDEBUG != 0
  if (yydebug)
    {
      int i;

      fprintf (stderr, "Reducing via rule %d (line %d), ",
	       yyn, yyrline[yyn]);

      /* Print the symbols being reduced, and their result.  */
      for (i = yyprhs[yyn]; yyrhs[i] > 0; i++)
	fprintf (stderr, "%s ", yytname[yyrhs[i]]);
      fprintf (stderr, " -> %s\n", yytname[yyr1[yyn]]);
    }
#endif


  switch (yyn) {

case 2:
#line 193 "BisonMe.y"
{s_DEF_name(NULL);;
    break;}
case 3:
#line 194 "BisonMe.y"
{s_DEF_name(yyvsp[-1].sval);;
    break;}
case 4:
#line 197 "BisonMe.y"
{s_Separator_begin();;
    break;}
case 5:
#line 198 "BisonMe.y"
{s_Separator_end();;
    break;}
case 6:
#line 199 "BisonMe.y"
{s_InitCube();;
    break;}
case 7:
#line 200 "BisonMe.y"
{s_EndCube();;
    break;}
case 8:
#line 201 "BisonMe.y"
{s_InitCylinder();;
    break;}
case 9:
#line 202 "BisonMe.y"
{s_EndCylinder();;
    break;}
case 10:
#line 203 "BisonMe.y"
{s_Vertices(yyvsp[-1].vbuf); delete yyvsp[-1].vbuf;;
    break;}
case 11:
#line 204 "BisonMe.y"
{s_InitLight(0);;
    break;}
case 12:
#line 205 "BisonMe.y"
{s_EndLight();;
    break;}
case 13:
#line 206 "BisonMe.y"
{s_OutputIndexedFaceSet();;
    break;}
case 14:
#line 207 "BisonMe.y"
{;
    break;}
case 15:
#line 208 "BisonMe.y"
{;
    break;}
case 16:
#line 209 "BisonMe.y"
{s_MaterialBinding(yyvsp[-1].ival);;
    break;}
case 17:
#line 210 "BisonMe.y"
{;
    break;}
case 18:
#line 211 "BisonMe.y"
{s_Normals(yyvsp[-1].vbuf); delete yyvsp[-1].vbuf;;
    break;}
case 19:
#line 212 "BisonMe.y"
{s_NormalBinding(yyvsp[-1].ival);;
    break;}
case 20:
#line 213 "BisonMe.y"
{s_InitCamera(0);;
    break;}
case 21:
#line 214 "BisonMe.y"
{s_EndCamera();;
    break;}
case 22:
#line 215 "BisonMe.y"
{s_InitCamera(1);;
    break;}
case 23:
#line 216 "BisonMe.y"
{s_EndCamera();;
    break;}
case 24:
#line 217 "BisonMe.y"
{s_InitLight(1);;
    break;}
case 25:
#line 218 "BisonMe.y"
{s_EndLight();;
    break;}
case 26:
#line 219 "BisonMe.y"
{;
    break;}
case 27:
#line 220 "BisonMe.y"
{;
    break;}
case 28:
#line 221 "BisonMe.y"
{;
    break;}
case 29:
#line 222 "BisonMe.y"
{s_InitLight(2);;
    break;}
case 30:
#line 223 "BisonMe.y"
{s_EndLight();;
    break;}
case 31:
#line 224 "BisonMe.y"
{;
    break;}
case 32:
#line 225 "BisonMe.y"
{s_Tex2Tran_begin();;
    break;}
case 33:
#line 226 "BisonMe.y"
{s_Tex2Tran_end();;
    break;}
case 34:
#line 227 "BisonMe.y"
{s_TexCoords(yyvsp[-1].vbuf); delete yyvsp[-1].vbuf;;
    break;}
case 35:
#line 228 "BisonMe.y"
{;
    break;}
case 36:
#line 229 "BisonMe.y"
{;
    break;}
case 37:
#line 230 "BisonMe.y"
{;
    break;}
case 38:
#line 231 "BisonMe.y"
{;
    break;}
case 39:
#line 232 "BisonMe.y"
{;
    break;}
case 40:
#line 233 "BisonMe.y"
{;
    break;}
case 41:
#line 234 "BisonMe.y"
{;
    break;}
case 46:
#line 245 "BisonMe.y"
{;
    break;}
case 47:
#line 246 "BisonMe.y"
{/* Really part of WWWAnchor*/;
    break;}
case 48:
#line 247 "BisonMe.y"
{/* Really part of WWWAnchor*/;
    break;}
case 49:
#line 250 "BisonMe.y"
{yyval.ival = TR_ON;;
    break;}
case 50:
#line 251 "BisonMe.y"
{yyval.ival = TR_OFF;;
    break;}
case 51:
#line 252 "BisonMe.y"
{yyval.ival = TR_AUTO;;
    break;}
case 54:
#line 259 "BisonMe.y"
{s_Info(yyvsp[0].sval);;
    break;}
case 57:
#line 266 "BisonMe.y"
{s_CameraParam3(yyvsp[0].vec, 0, 1);;
    break;}
case 58:
#line 267 "BisonMe.y"
{s_CameraParam3(yyvsp[-1].vec, yyvsp[0].fval, 2);;
    break;}
case 59:
#line 268 "BisonMe.y"
{s_CameraParam1(yyvsp[0].fval, 3);;
    break;}
case 60:
#line 269 "BisonMe.y"
{s_CameraParam1(yyvsp[0].fval, 4);;
    break;}
case 61:
#line 270 "BisonMe.y"
{s_CameraParam1(yyvsp[0].fval, 4);;
    break;}
case 64:
#line 277 "BisonMe.y"
{s_LightParam1(yyvsp[0].ival, 1);;
    break;}
case 65:
#line 278 "BisonMe.y"
{s_LightParam1(yyvsp[0].fval, 2);;
    break;}
case 66:
#line 279 "BisonMe.y"
{s_LightParam3(yyvsp[0].vec, 3);;
    break;}
case 67:
#line 280 "BisonMe.y"
{s_LightParam3(yyvsp[0].vec, 4);;
    break;}
case 68:
#line 281 "BisonMe.y"
{s_LightParam3(yyvsp[0].vec, 5);;
    break;}
case 69:
#line 282 "BisonMe.y"
{s_LightParam1(yyvsp[0].fval, 6);;
    break;}
case 70:
#line 283 "BisonMe.y"
{s_LightParam1(yyvsp[0].fval, 7);;
    break;}
case 73:
#line 290 "BisonMe.y"
{;
    break;}
case 74:
#line 291 "BisonMe.y"
{;
    break;}
case 75:
#line 292 "BisonMe.y"
{;
    break;}
case 76:
#line 293 "BisonMe.y"
{;
    break;}
case 77:
#line 296 "BisonMe.y"
{s_Center(yyvsp[0].vec);;
    break;}
case 78:
#line 299 "BisonMe.y"
{s_Scale(yyvsp[0].vec);;
    break;}
case 79:
#line 302 "BisonMe.y"
{s_Rotation(yyvsp[0].fval);;
    break;}
case 80:
#line 305 "BisonMe.y"
{s_Translation(yyvsp[0].vec);;
    break;}
case 81:
#line 308 "BisonMe.y"
{s_Scale(yyvsp[0].vec);;
    break;}
case 82:
#line 311 "BisonMe.y"
{s_Rotation(yyvsp[0].fval, yyvsp[-1].vec);;
    break;}
case 83:
#line 314 "BisonMe.y"
{s_Translation(yyvsp[0].vec);;
    break;}
case 84:
#line 317 "BisonMe.y"
{yyval.ival = PER_VERTEX_INDEXED;
    break;}
case 85:
#line 318 "BisonMe.y"
{yyval.ival = PER_FACE_INDEXED;
    break;}
case 86:
#line 319 "BisonMe.y"
{yyval.ival = OVERALL;
    break;}
case 87:
#line 320 "BisonMe.y"
{yyval.ival = PER_FACE;
    break;}
case 88:
#line 321 "BisonMe.y"
{yyval.ival = PER_VERTEX;
    break;}
case 89:
#line 324 "BisonMe.y"
{yyval.ival = PER_VERTEX_INDEXED;
    break;}
case 90:
#line 325 "BisonMe.y"
{yyval.ival = PER_FACE_INDEXED;
    break;}
case 91:
#line 326 "BisonMe.y"
{yyval.ival = OVERALL;
    break;}
case 92:
#line 327 "BisonMe.y"
{yyval.ival = PER_FACE;
    break;}
case 93:
#line 328 "BisonMe.y"
{yyval.ival = PER_VERTEX;
    break;}
case 96:
#line 335 "BisonMe.y"
{s_ObjWidth(yyvsp[0].fval);;
    break;}
case 97:
#line 336 "BisonMe.y"
{s_ObjHeight(yyvsp[0].fval);;
    break;}
case 98:
#line 337 "BisonMe.y"
{s_ObjDepth(yyvsp[0].fval);;
    break;}
case 101:
#line 344 "BisonMe.y"
{s_ObjRadius(yyvsp[0].fval);;
    break;}
case 102:
#line 345 "BisonMe.y"
{s_ObjHeight(yyvsp[0].fval);;
    break;}
case 105:
#line 354 "BisonMe.y"
{;
    break;}
case 106:
#line 355 "BisonMe.y"
{;
    break;}
case 107:
#line 356 "BisonMe.y"
{;
    break;}
case 110:
#line 363 "BisonMe.y"
{;
    break;}
case 111:
#line 364 "BisonMe.y"
{;
    break;}
case 112:
#line 365 "BisonMe.y"
{;
    break;}
case 113:
#line 366 "BisonMe.y"
{;
    break;}
case 116:
#line 373 "BisonMe.y"
{;
    break;}
case 117:
#line 374 "BisonMe.y"
{;
    break;}
case 118:
#line 375 "BisonMe.y"
{;
    break;}
case 121:
#line 382 "BisonMe.y"
{;
    break;}
case 122:
#line 383 "BisonMe.y"
{;
    break;}
case 123:
#line 384 "BisonMe.y"
{;
    break;}
case 126:
#line 391 "BisonMe.y"
{;
    break;}
case 127:
#line 392 "BisonMe.y"
{;
    break;}
case 128:
#line 393 "BisonMe.y"
{;
    break;}
case 131:
#line 400 "BisonMe.y"
{;
    break;}
case 132:
#line 401 "BisonMe.y"
{;
    break;}
case 133:
#line 404 "BisonMe.y"
{;
    break;}
case 134:
#line 405 "BisonMe.y"
{;
    break;}
case 135:
#line 407 "BisonMe.y"
{;
    break;}
case 136:
#line 408 "BisonMe.y"
{;
    break;}
case 137:
#line 411 "BisonMe.y"
{;
    break;}
case 138:
#line 412 "BisonMe.y"
{;
    break;}
case 139:
#line 413 "BisonMe.y"
{;
    break;}
case 140:
#line 414 "BisonMe.y"
{;
    break;}
case 141:
#line 415 "BisonMe.y"
{;
    break;}
case 142:
#line 416 "BisonMe.y"
{;
    break;}
case 143:
#line 417 "BisonMe.y"
{;
    break;}
case 144:
#line 418 "BisonMe.y"
{;
    break;}
case 145:
#line 419 "BisonMe.y"
{;
    break;}
case 148:
#line 428 "BisonMe.y"
{;
    break;}
case 149:
#line 429 "BisonMe.y"
{;
    break;}
case 150:
#line 430 "BisonMe.y"
{;
    break;}
case 151:
#line 431 "BisonMe.y"
{;
    break;}
case 152:
#line 432 "BisonMe.y"
{s_CreaseAngle(yyvsp[0].fval);;
    break;}
case 153:
#line 435 "BisonMe.y"
{;
    break;}
case 154:
#line 436 "BisonMe.y"
{;
    break;}
case 155:
#line 439 "BisonMe.y"
{s_Matrix(yyvsp[0].mptr); delete [] yyvsp[0].mptr;;
    break;}
case 156:
#line 446 "BisonMe.y"
{yyval.mptr = new double[16];
	             ASSERTERR(yyval.mptr, "new matrix failed");
		     yyval.mptr[0] = yyvsp[-15].fval;   yyval.mptr[1] = yyvsp[-14].fval;   yyval.mptr[2] = yyvsp[-13].fval;   yyval.mptr[3] = yyvsp[-12].fval;
                     yyval.mptr[4] = yyvsp[-11].fval;   yyval.mptr[5] = yyvsp[-10].fval;   yyval.mptr[6] = yyvsp[-9].fval;   yyval.mptr[7] = yyvsp[-8].fval;
                     yyval.mptr[8] = yyvsp[-7].fval;   yyval.mptr[9] = yyvsp[-6].fval;  yyval.mptr[10] = yyvsp[-5].fval; yyval.mptr[11] = yyvsp[-4].fval;
                     yyval.mptr[12] = yyvsp[-3].fval; yyval.mptr[13] = yyvsp[-2].fval; yyval.mptr[14] = yyvsp[-1].fval; yyval.mptr[15] = yyvsp[0].fval;;
    break;}
case 157:
#line 454 "BisonMe.y"
{s_Texture2_filename(yyvsp[0].sval);;
    break;}
case 158:
#line 455 "BisonMe.y"
{s_Texture2_filename("");;
    break;}
case 159:
#line 458 "BisonMe.y"
{yyval.vbuf = yyvsp[-1].vbuf;;
    break;}
case 160:
#line 459 "BisonMe.y"
{yyval.vbuf = yyvsp[0].vbuf;;
    break;}
case 161:
#line 462 "BisonMe.y"
{yyval.vbuf = yyvsp[-1].vbuf;;
    break;}
case 162:
#line 463 "BisonMe.y"
{yyval.vbuf = yyvsp[0].vbuf;;
    break;}
case 163:
#line 466 "BisonMe.y"
{yyval.vbuf = yyvsp[-1].vbuf;;
    break;}
case 164:
#line 467 "BisonMe.y"
{yyval.vbuf = yyvsp[0].vbuf;;
    break;}
case 167:
#line 474 "BisonMe.y"
{s_CoordIndices(yyvsp[-1].ibuf);;
    break;}
case 168:
#line 475 "BisonMe.y"
{s_MaterialIndices(yyvsp[-1].ibuf);;
    break;}
case 169:
#line 476 "BisonMe.y"
{s_NormalIndices(yyvsp[-1].ibuf);;
    break;}
case 170:
#line 477 "BisonMe.y"
{s_TexCoordIndices(yyvsp[-1].ibuf);;
    break;}
case 173:
#line 484 "BisonMe.y"
{s_AmbientColors(yyvsp[0].vbuf); delete yyvsp[0].vbuf;;
    break;}
case 174:
#line 485 "BisonMe.y"
{s_DiffuseColors(yyvsp[0].vbuf); delete yyvsp[0].vbuf;;
    break;}
case 175:
#line 486 "BisonMe.y"
{s_EmissiveColors(yyvsp[0].vbuf); delete yyvsp[0].vbuf;;
    break;}
case 176:
#line 487 "BisonMe.y"
{s_SpecularColors(yyvsp[0].vbuf); delete yyvsp[0].vbuf;;
    break;}
case 177:
#line 488 "BisonMe.y"
{s_Shininesses(yyvsp[0].fbuf); delete yyvsp[0].fbuf;;
    break;}
case 178:
#line 489 "BisonMe.y"
{s_Transparencies(yyvsp[0].fbuf); delete yyvsp[0].fbuf;;
    break;}
case 179:
#line 492 "BisonMe.y"
{yyval.vbuf = yyvsp[-1].vbuf;;
    break;}
case 180:
#line 493 "BisonMe.y"
{yyval.vbuf = yyvsp[0].vbuf;;
    break;}
case 181:
#line 496 "BisonMe.y"
{yyval.vbuf = yyvsp[-1].vbuf;;
    break;}
case 182:
#line 497 "BisonMe.y"
{yyval.vbuf = yyvsp[0].vbuf;;
    break;}
case 183:
#line 500 "BisonMe.y"
{yyval.vbuf = yyvsp[-1].vbuf;;
    break;}
case 184:
#line 501 "BisonMe.y"
{yyval.vbuf = yyvsp[0].vbuf;;
    break;}
case 185:
#line 504 "BisonMe.y"
{yyval.vbuf = yyvsp[-1].vbuf;;
    break;}
case 186:
#line 505 "BisonMe.y"
{yyval.vbuf = yyvsp[0].vbuf;;
    break;}
case 187:
#line 508 "BisonMe.y"
{yyval.fbuf = yyvsp[-1].fbuf;;
    break;}
case 188:
#line 509 "BisonMe.y"
{yyval.fbuf = yyvsp[0].fbuf;;
    break;}
case 189:
#line 512 "BisonMe.y"
{yyval.fbuf = yyvsp[-1].fbuf;;
    break;}
case 190:
#line 513 "BisonMe.y"
{yyval.fbuf = yyvsp[0].fbuf;;
    break;}
case 191:
#line 516 "BisonMe.y"
{yyval.vbuf = yyvsp[0].vbuf;
    break;}
case 192:
#line 517 "BisonMe.y"
{yyval.vbuf = yyvsp[-1].vbuf;
    break;}
case 193:
#line 520 "BisonMe.y"
{yyval.vbuf=new vector<Vector>; yyval.vbuf->push_back(*yyvsp[0].vec); delete yyvsp[0].vec;;
    break;}
case 194:
#line 521 "BisonMe.y"
{yyval.vbuf = yyvsp[-2].vbuf; yyval.vbuf->push_back(*yyvsp[0].vec); delete yyvsp[0].vec;;
    break;}
case 195:
#line 524 "BisonMe.y"
{yyval.vbuf=new vector<Vector>; yyval.vbuf->push_back(Vector(yyvsp[-2].fval, yyvsp[-1].fval, yyvsp[0].fval));;
    break;}
case 196:
#line 527 "BisonMe.y"
{yyval.vec = new Vector(yyvsp[-2].fval, yyvsp[-1].fval, yyvsp[0].fval);;
    break;}
case 197:
#line 530 "BisonMe.y"
{yyval.vbuf = yyvsp[0].vbuf;
    break;}
case 198:
#line 531 "BisonMe.y"
{yyval.vbuf = yyvsp[-1].vbuf;
    break;}
case 199:
#line 534 "BisonMe.y"
{yyval.vbuf=new vector<Vector>; yyval.vbuf->push_back(*yyvsp[0].vec); delete yyvsp[0].vec;;
    break;}
case 200:
#line 535 "BisonMe.y"
{yyval.vbuf = yyvsp[-2].vbuf; yyval.vbuf->push_back(*yyvsp[0].vec); delete yyvsp[0].vec;;
    break;}
case 201:
#line 538 "BisonMe.y"
{yyval.vbuf=new vector<Vector>; yyval.vbuf->push_back(Vector(yyvsp[-1].fval, yyvsp[0].fval, 0));;
    break;}
case 202:
#line 541 "BisonMe.y"
{yyval.vec = new Vector(yyvsp[-1].fval, yyvsp[0].fval, 0);;
    break;}
case 203:
#line 544 "BisonMe.y"
{yyval.ibuf = yyvsp[0].ibuf;
    break;}
case 204:
#line 545 "BisonMe.y"
{yyval.ibuf = yyvsp[-1].ibuf;
    break;}
case 205:
#line 546 "BisonMe.y"
{yyval.ibuf = new vector<int>;;
    break;}
case 206:
#line 549 "BisonMe.y"
{yyval.ibuf = new vector<int>; yyval.ibuf->push_back(yyvsp[0].ival);;
    break;}
case 207:
#line 550 "BisonMe.y"
{yyval.ibuf = yyvsp[-2].ibuf; yyval.ibuf->push_back(yyvsp[0].ival);;
    break;}
case 208:
#line 553 "BisonMe.y"
{yyval.ival = yyvsp[0].ival;;
    break;}
case 209:
#line 555 "BisonMe.y"
{yyval.fbuf = yyvsp[0].fbuf;
    break;}
case 210:
#line 556 "BisonMe.y"
{yyval.fbuf = yyvsp[-1].fbuf;
    break;}
case 211:
#line 559 "BisonMe.y"
{yyval.fbuf = new vector<double>; yyval.fbuf->push_back(yyvsp[0].fval);;
    break;}
case 212:
#line 560 "BisonMe.y"
{yyval.fbuf = yyvsp[-2].fbuf; yyval.fbuf->push_back(yyvsp[0].fval);;
    break;}
case 213:
#line 563 "BisonMe.y"
{yyval.fbuf = new vector<double>; yyval.fbuf->push_back(yyvsp[0].fval);;
    break;}
case 214:
#line 566 "BisonMe.y"
{yyval.ival = (int) (yyvsp[0].ival) ? 1 : 0;;
    break;}
case 215:
#line 567 "BisonMe.y"
{yyval.ival = 1;;
    break;}
case 216:
#line 568 "BisonMe.y"
{yyval.ival = 0;;
    break;}
case 217:
#line 571 "BisonMe.y"
{yyval.fval = (double) yyvsp[0].ival;;
    break;}
case 218:
#line 572 "BisonMe.y"
{yyval.fval = yyvsp[0].fval;;
    break;}
}
   /* the action file gets copied in in place of this dollarsign */
#line 498 "bison.simple"

  yyvsp -= yylen;
  yyssp -= yylen;
#ifdef YYLSP_NEEDED
  yylsp -= yylen;
#endif

#if YYDEBUG != 0
  if (yydebug)
    {
      short *ssp1 = yyss - 1;
      fprintf (stderr, "state stack now");
      while (ssp1 != yyssp)
	fprintf (stderr, " %d", *++ssp1);
      fprintf (stderr, "\n");
    }
#endif

  *++yyvsp = yyval;

#ifdef YYLSP_NEEDED
  yylsp++;
  if (yylen == 0)
    {
      yylsp->first_line = yylloc.first_line;
      yylsp->first_column = yylloc.first_column;
      yylsp->last_line = (yylsp-1)->last_line;
      yylsp->last_column = (yylsp-1)->last_column;
      yylsp->text = 0;
    }
  else
    {
      yylsp->last_line = (yylsp+yylen-1)->last_line;
      yylsp->last_column = (yylsp+yylen-1)->last_column;
    }
#endif

  /* Now "shift" the result of the reduction.
     Determine what state that goes to,
     based on the state we popped back to
     and the rule number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTBASE] + *yyssp;
  if (yystate >= 0 && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTBASE];

  goto yynewstate;

yyerrlab:   /* here on detecting error */

  if (! yyerrstatus)
    /* If not already recovering from an error, report this error.  */
    {
      ++yynerrs;

#ifdef YYERROR_VERBOSE
      yyn = yypact[yystate];

      if (yyn > YYFLAG && yyn < YYLAST)
	{
	  int size = 0;
	  char *msg;
	  int x, count;

	  count = 0;
	  /* Start X at -yyn if nec to avoid negative indexes in yycheck.  */
	  for (x = (yyn < 0 ? -yyn : 0);
	       x < (sizeof(yytname) / sizeof(char *)); x++)
	    if (yycheck[x + yyn] == x)
	      size += int(strlen(yytname[x])) + 15, count++;
	  msg = (char *) malloc(size + 15);
	  if (msg != 0)
	    {
	      strcpy(msg, "parse error");

	      if (count < 5)
		{
		  count = 0;
		  for (x = (yyn < 0 ? -yyn : 0);
		       x < (sizeof(yytname) / sizeof(char *)); x++)
		    if (yycheck[x + yyn] == x)
		      {
			strcat(msg, count == 0 ? ", expecting `" : " or `");
			strcat(msg, yytname[x]);
			strcat(msg, "'");
			count++;
		      }
		}
	      yyerror(msg);
	      free(msg);
	    }
	  else
	    yyerror ("parse error; also virtual memory exceeded");
	}
      else
#endif /* YYERROR_VERBOSE */
	yyerror("parse error");
    }

  goto yyerrlab1;
yyerrlab1:   /* here on error raised explicitly by an action */

  if (yyerrstatus == 3)
    {
      /* if just tried and failed to reuse lookahead token after an error, discard it.  */

      /* return failure if at end of input */
      if (yychar == YYEOF)
	YYABORT;

#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Discarding token %d (%s).\n", yychar, yytname[yychar1]);
#endif

      yychar = YYEMPTY;
    }

  /* Else will try to reuse lookahead token
     after shifting the error token.  */

  yyerrstatus = 3;		/* Each real token shifted decrements this */

  goto yyerrhandle;

yyerrdefault:  /* current state does not do anything special for the error token. */

#if 0
  /* This is wrong; only states that explicitly want error tokens
     should shift them.  */
  yyn = yydefact[yystate];  /* If its default is to accept any token, ok.  Otherwise pop it.*/
  if (yyn) goto yydefault;
#endif

yyerrpop:   /* pop the current state because it cannot handle the error token */

  if (yyssp == yyss) YYABORT;
  yyvsp--;
  yystate = *--yyssp;
#ifdef YYLSP_NEEDED
  yylsp--;
#endif

#if YYDEBUG != 0
  if (yydebug)
    {
      short *ssp1 = yyss - 1;
      fprintf (stderr, "Error: state stack now");
      while (ssp1 != yyssp)
	fprintf (stderr, " %d", *++ssp1);
      fprintf (stderr, "\n");
    }
#endif

yyerrhandle:

  yyn = yypact[yystate];
  if (yyn == YYFLAG)
    goto yyerrdefault;

  yyn += YYTERROR;
  if (yyn < 0 || yyn > YYLAST || yycheck[yyn] != YYTERROR)
    goto yyerrdefault;

  yyn = yytable[yyn];
  if (yyn < 0)
    {
      if (yyn == YYFLAG)
	goto yyerrpop;
      yyn = -yyn;
      goto yyreduce;
    }
  else if (yyn == 0)
    goto yyerrpop;

  if (yyn == YYFINAL)
    YYACCEPT;

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Shifting error token, ");
#endif

  *++yyvsp = yylval;
#ifdef YYLSP_NEEDED
  *++yylsp = yylloc;
#endif

  yystate = yyn;
  goto yynewstate;
}
#line 575 "BisonMe.y"

/* get yylex() from FlexMe.cpp */
#include <Model/FlexMe.cpp>

