typedef union {
    int none; /* Used to flag values w/o type */
    int ival;
    f3Vector::ElType fval;
    char* sval;
    f3Vector::ElType* mptr;
    f3Vector* vec;
    std::vector<int>* ibuf;
    std::vector<f3Vector::ElType>* fbuf;
    std::vector<f3Vector>* vbuf;
} YYSTYPE;
#define T_ERROR 258
#define T_INT 259
#define T_FLOAT 260
#define T_STRING 261
#define TR_DEF 262
#define TR_Separator 263
#define TR_name 264
#define TR_map 265
#define TR_NONE 266
#define TR_Info 267
#define TR_string 268
#define TR_PerspectiveCamera 269
#define TR_OrthographicCamera 270
#define TR_position 271
#define TR_orientation 272
#define TR_focalDistance 273
#define TR_heightAngle 274
#define TR_MaterialBinding 275
#define TR_NormalBinding 276
#define TR_value 277
#define TR_OVERALL 278
#define TR_PER_FACE 279
#define TR_PER_FACE_INDEXED 280
#define TR_PER_VERTEX 281
#define TR_PER_VERTEX_INDEXED 282
#define TR_ShapeHints 283
#define TR_vertexOrdering 284
#define TR_COUNTERCLOCKWISE 285
#define TR_CLOCKWISE 286
#define TR_shapeType 287
#define TR_SOLID 288
#define TR_UNKNOWN_SHAPE_TYPE 289
#define TR_faceType 290
#define TR_CONVEX 291
#define TR_creaseAngle 292
#define TR_MatrixTransform 293
#define TR_matrix 294
#define TR_Scale 295
#define TR_scaleFactor 296
#define TR_Rotation 297
#define TR_rotation 298
#define TR_Translation 299
#define TR_translation 300
#define TR_Texture2Transform 301
#define TR_center 302
#define TR_renderCulling 303
#define TR_ON 304
#define TR_OFF 305
#define TR_AUTO 306
#define TR_Texture2 307
#define TR_filename 308
#define TR_Coordinate3 309
#define TR_point 310
#define TR_Normal 311
#define TR_vector 312
#define TR_Material 313
#define TR_ambientColor 314
#define TR_specularColor 315
#define TR_emissiveColor 316
#define TR_diffuseColor 317
#define TR_shininess 318
#define TR_transparency 319
#define TR_TextureCoordinate2 320
#define TR_IndexedFaceSet 321
#define TR_coordIndex 322
#define TR_materialIndex 323
#define TR_normalIndex 324
#define TR_textureCoordIndex 325
#define TR_Cylinder 326
#define TR_Cube 327
#define TR_radius 328
#define TR_width 329
#define TR_height 330
#define TR_depth 331
#define TR_DirectionalLight 332
#define TR_PointLight 333
#define TR_SpotLight 334
#define TR_intensity 335
#define TR_color 336
#define TR_direction 337
#define TR_location 338
#define TR_on 339
#define TR_dropOffRate 340
#define TR_cutOffAngle 341
#define TR_TRUE 342
#define TR_FALSE 343
#define TR_ScaleKey_ktx_com 344
#define TR_scale 345
#define TR_RotationKey_ktx_com 346
#define TR_fields 347
#define TR_frame 348
#define TR_AnimationStyle_ktx_com 349
#define TR_AnimationFrames_ktx_com 350
#define TR_splineUse 351
#define TR_pivotOffset 352
#define TR_PositionKey_ktx_com 353
#define TR_AimTarget_ktx_com 354
#define TR_aimer 355
#define TR_rate 356
#define TR_length 357

extern YYSTYPE yylval;
