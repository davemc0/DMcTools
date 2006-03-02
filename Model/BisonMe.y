%{
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

%}

%union {
  int		none; /* Used to flag values w/o type */
  int		ival;
  double	fval;
  char		*sval;
  double	*mptr;
  Vector	*vec;
  vector<int>	*ibuf;
  vector<double> *fbuf;
  vector<Vector> *vbuf;
}

%{
/* error */
%}
%token <none> T_ERROR

%{
/* literals */
%}
%token <ival> T_INT
%token <fval> T_FLOAT
%token <sval> T_STRING

%{
/* Keywords.  All begin with TR_ */
%}
%token <none> TR_DEF
%token <none> TR_Separator
%token <none> TR_name
%token <none> TR_map
%token <none> TR_NONE
%token <none> TR_Info
%token <none> TR_string
%token <none> TR_PerspectiveCamera
%token <none> TR_OrthographicCamera
%token <none> TR_position
%token <none> TR_orientation
%token <none> TR_focalDistance
%token <none> TR_heightAngle
%token <none> TR_MaterialBinding
%token <none> TR_NormalBinding
%token <none> TR_value
%token <none> TR_OVERALL
%token <none> TR_PER_FACE
%token <none> TR_PER_FACE_INDEXED
%token <none> TR_PER_VERTEX
%token <none> TR_PER_VERTEX_INDEXED
%token <none> TR_ShapeHints
%token <none> TR_vertexOrdering
%token <none> TR_COUNTERCLOCKWISE
%token <none> TR_CLOCKWISE
%token <none> TR_shapeType
%token <none> TR_SOLID
%token <none> TR_UNKNOWN_SHAPE_TYPE
%token <none> TR_faceType
%token <none> TR_CONVEX
%token <none> TR_creaseAngle
%token <none> TR_MatrixTransform
%token <none> TR_matrix
%token <none> TR_Scale
%token <none> TR_scaleFactor
%token <none> TR_Rotation
%token <none> TR_rotation
%token <none> TR_Translation
%token <none> TR_translation
%token <none> TR_Texture2Transform
%token <none> TR_center
%token <none> TR_renderCulling
%token <none> TR_ON
%token <none> TR_OFF
%token <none> TR_AUTO
%token <none> TR_Texture2
%token <none> TR_filename
%token <none> TR_Coordinate3
%token <none> TR_point
%token <none> TR_Normal
%token <none> TR_vector
%token <none> TR_Material
%token <none> TR_ambientColor
%token <none> TR_specularColor
%token <none> TR_emissiveColor
%token <none> TR_diffuseColor
%token <none> TR_shininess
%token <none> TR_transparency
%token <none> TR_TextureCoordinate2
%token <none> TR_IndexedFaceSet
%token <none> TR_coordIndex
%token <none> TR_materialIndex
%token <none> TR_normalIndex
%token <none> TR_textureCoordIndex
%token <none> TR_Cylinder
%token <none> TR_Cube
%token <none> TR_radius
%token <none> TR_width
%token <none> TR_height
%token <none> TR_depth
%token <none> TR_DirectionalLight
%token <none> TR_PointLight
%token <none> TR_SpotLight
%token <none> TR_intensity
%token <none> TR_color
%token <none> TR_direction
%token <none> TR_location
%token <none> TR_on
%token <none> TR_dropOffRate
%token <none> TR_cutOffAngle
%token <none> TR_TRUE
%token <none> TR_FALSE

%token <none> TR_ScaleKey_ktx_com
%token <none> TR_scale
%token <none> TR_RotationKey_ktx_com
%token <none> TR_fields
%token <none> TR_frame
%token <none> TR_AnimationStyle_ktx_com
%token <none> TR_AnimationFrames_ktx_com
%token <none> TR_splineUse
%token <none> TR_pivotOffset
%token <none> TR_PositionKey_ktx_com
%token <none> TR_AimTarget_ktx_com
%token <none> TR_aimer
%token <none> TR_rate
%token <none> TR_length

%type <ival> rcopt
%type <ival> sf_bool
%type <mptr> matrix4x4

%type <vbuf> c3_field
%type <vbuf> v3_field
%type <vbuf> acolor
%type <vbuf> dcolor
%type <vbuf> ecolor
%type <vbuf> scolor
%type <fbuf> shine
%type <fbuf> transp
%type <vbuf> tc2_field
%type <ival> mb_field
%type <ival> nb_field

%type <vec>  triple
%type <vbuf> triples
%type <vbuf> onetriple
%type <vbuf> rtriples
%type <vec>  double
%type <vbuf> doubles
%type <vbuf> onedouble
%type <vbuf> rdoubles
%type <ival> isingle
%type <ibuf> isingles
%type <ibuf> risingles
%type <fval> fnum
%type <fbuf> fsingles
%type <fbuf> onefsingle
%type <fbuf> rfsingles

%%

vrmlfile	: node
		;

node		: rnode					    	{s_DEF_name(NULL);}
		| TR_DEF T_STRING rnode				{s_DEF_name($2);}
		;

rnode		: TR_Separator '{'				{s_Separator_begin();}
                  separator_fields nodes '}' {s_Separator_end();}
		| TR_Cube '{'					    {s_InitCube();}
		  cube_fields '}'			    	{s_EndCube();}
		| TR_Cylinder '{'					{s_InitCylinder();}
		  cylinder_fields '}'				{s_EndCylinder();}
		| TR_Coordinate3 '{' c3_field '}'	{s_Vertices($3); delete $3;}
		| TR_DirectionalLight '{'           {s_InitLight(0);}
          light_fields '}'                  {s_EndLight();}
		| TR_IndexedFaceSet '{' ifs_fields '}'		{s_OutputIndexedFaceSet();}
		| TR_Info '{' info_fields '}'			{}
		| TR_Material '{' mat_fields '}'		{}
		| TR_MaterialBinding '{' mb_field '}'		{s_MaterialBinding($3);}
		| TR_MatrixTransform '{' mt_field '}'		{}
		| TR_Normal '{' v3_field '}'			{s_Normals($3); delete $3;}
		| TR_NormalBinding '{' nb_field '}'		{s_NormalBinding($3);}
		| TR_OrthographicCamera '{'          {s_InitCamera(0);}
          cam_fields '}'                     {s_EndCamera();}
		| TR_PerspectiveCamera '{'           {s_InitCamera(1);}
          cam_fields '}'                     {s_EndCamera();}
		| TR_PointLight '{'           {s_InitLight(1);}
          light_fields '}'                  {s_EndLight();}
		| TR_Rotation '{' ro_field '}'			{}
		| TR_Scale '{' sc_field '}'			{}
		| TR_ShapeHints '{' sh_fields '}'		{}
		| TR_SpotLight '{'           {s_InitLight(2);}
          light_fields '}'                  {s_EndLight();}
		| TR_Texture2 '{' t2_field '}'			{}
		| TR_Texture2Transform '{'			{s_Tex2Tran_begin();}
		  t2t_fields '}'				{s_Tex2Tran_end();}
		| TR_TextureCoordinate2 '{' tc2_field '}'	{s_TexCoords($3); delete $3;}
		| TR_Translation '{' tr_field '}'		{}
		| TR_RotationKey_ktx_com '{' rot_ktx_fields '}'	{}
		| TR_ScaleKey_ktx_com '{' sca_ktx_fields '}'	{}
		| TR_PositionKey_ktx_com '{' pos_ktx_fields '}'	{}
		| TR_AnimationStyle_ktx_com '{' ani_ktx_fields '}'	{}
		| TR_AnimationFrames_ktx_com '{' anif_ktx_fields '}'	{}
		| TR_AimTarget_ktx_com '{' aim_ktx_fields '}'	{}
		;

nodes		: nodes node
		|
		;

separator_fields: separator_fields separator_field
		|
		;

separator_field	: TR_renderCulling rcopt			{}
		| TR_name T_STRING				{/* Really part of WWWAnchor*/}
		| TR_map TR_NONE				{/* Really part of WWWAnchor*/}
		;

rcopt		: TR_ON		{$$ = TR_ON;}
		| TR_OFF	{$$ = TR_OFF;}
		| TR_AUTO	{$$ = TR_AUTO;}
		;

info_fields	: info_fields info_field
		|
		;

info_field	: TR_string T_STRING {s_Info($2);}
		;

cam_fields	: cam_fields cam_field
		|
		;

cam_field	: TR_position      triple		{s_CameraParam3($2, 0, 1);}
		| TR_orientation   triple fnum		{s_CameraParam3($2, $3, 2);}
		| TR_focalDistance fnum				{s_CameraParam1($2, 3);}
		| TR_heightAngle   fnum				{s_CameraParam1($2, 4);}
		| TR_height        fnum				{s_CameraParam1($2, 4);}
		;

light_fields	: light_fields light_field
		|
		;

light_field    	: TR_on sf_bool			{s_LightParam1($2, 1);}
		| TR_intensity fnum			    {s_LightParam1($2, 2);}
		| TR_color triple			    {s_LightParam3($2, 3);}
		| TR_direction triple			{s_LightParam3($2, 4);}
		| TR_location triple			{s_LightParam3($2, 5);}
		| TR_dropOffRate fnum			{s_LightParam1($2, 6);}
		| TR_cutOffAngle fnum			{s_LightParam1($2, 7);}
		;

t2t_fields	: t2t_fields t2t_field
		|
		;

t2t_field	: sc2_field				{}
		| ro2_field				{}
		| tr2_field				{}
		| ce2_field				{}
		;

ce2_field	: TR_center double				{s_Center($2);}
		;

sc2_field	: TR_scaleFactor double				{s_Scale($2);}
		;

ro2_field	: TR_rotation fnum				{s_Rotation($2);}
		;

tr2_field	: TR_translation double				{s_Translation($2);}
		;

sc_field	: TR_scaleFactor triple				{s_Scale($2);}
		;

ro_field	: TR_rotation triple fnum			{s_Rotation($3, $2);}
		;

tr_field	: TR_translation triple				{s_Translation($2);}
		;

mb_field	: TR_value TR_PER_VERTEX_INDEXED		{$$ = PER_VERTEX_INDEXED}
		| TR_value TR_PER_FACE_INDEXED			{$$ = PER_FACE_INDEXED}
		| TR_value TR_OVERALL				{$$ = OVERALL}
		| TR_value TR_PER_FACE				{$$ = PER_FACE}
		| TR_value TR_PER_VERTEX			{$$ = PER_VERTEX}
		;

nb_field	: TR_value TR_PER_VERTEX_INDEXED		{$$ = PER_VERTEX_INDEXED}
		| TR_value TR_PER_FACE_INDEXED			{$$ = PER_FACE_INDEXED}
		| TR_value TR_OVERALL				{$$ = OVERALL}
		| TR_value TR_PER_FACE				{$$ = PER_FACE}
		| TR_value TR_PER_VERTEX			{$$ = PER_VERTEX}
		;

cube_fields	: cube_fields cube_field
		|
		;

cube_field	: TR_width fnum					{s_ObjWidth($2);}
		| TR_height fnum				{s_ObjHeight($2);}
		| TR_depth fnum					{s_ObjDepth($2);}
		;

cylinder_fields	: cylinder_fields cylinder_field
		|
		;

cylinder_field	: TR_radius fnum		{s_ObjRadius($2);}
		| TR_height fnum				{s_ObjHeight($2);}
		;

/* This is so it can recognize 3D Studio Max extensions. */

rot_ktx_fields	: rot_ktx_fields rot_ktx_field
		|
		;

rot_ktx_field	: TR_fields '[' ext_fields ']'			{}
		| TR_frame isingle				{}
		| ro_field					{}
		;

sca_ktx_fields	: sca_ktx_fields sca_ktx_field
		|
		;

sca_ktx_field	: TR_fields '[' ext_fields ']'			{}
		| TR_scale triple				{}
		| TR_frame isingle				{}
		| ro_field					{}
		;

pos_ktx_fields	: pos_ktx_fields pos_ktx_field
		|
		;

pos_ktx_field	: TR_fields '[' ext_fields ']'			{}
		| TR_frame isingle				{}
		| tr_field					{}
		;

ani_ktx_fields	: ani_ktx_fields ani_ktx_field
		|
		;

ani_ktx_field	: TR_fields '[' ext_fields ']'			{}
		| TR_splineUse					{}
		| TR_pivotOffset triple				{}
		;

anif_ktx_fields	: anif_ktx_fields anif_ktx_field
		|
		;

anif_ktx_field	: TR_fields '[' ext_fields ']'			{}
		| TR_length isingle				{}
		| TR_rate isingle				{}
		;

aim_ktx_fields	: aim_ktx_fields aim_ktx_field
		|
		;

aim_ktx_field	: TR_fields '[' ext_fields ']'			{}
		| TR_aimer T_STRING				{}
		;

ext_fields	: ext_field					{}
		| ext_fields ',' ext_field			{}

ext_field	: T_STRING T_STRING				{}
		| T_STRING known_field_name			{}
		;

known_field_name : TR_rotation					{}
		| TR_translation				{}
		| TR_pivotOffset				{}
		| TR_splineUse					{}
		| TR_frame					{}
		| TR_aimer					{}
		| TR_scale					{}
		| TR_length					{}
		| TR_rate					{}
		;

/* End 3DSMAX extensions. */

sh_fields	: sh_fields sh_field
		|
		;

sh_field	: TR_vertexOrdering TR_COUNTERCLOCKWISE		{}
		| TR_vertexOrdering TR_CLOCKWISE		{}
		| TR_shapeType	    shape_type			{}
		| TR_faceType	    TR_CONVEX			{}
		| TR_creaseAngle    fnum			{s_CreaseAngle($2);}
		;

shape_type	: TR_SOLID					{}
		| TR_UNKNOWN_SHAPE_TYPE				{}
		;

mt_field	: TR_matrix matrix4x4		   {s_Matrix($2); delete [] $2;}
		;

matrix4x4	: fnum fnum fnum fnum
	          fnum fnum fnum fnum
	          fnum fnum fnum fnum
	          fnum fnum fnum fnum
                    {$$ = new double[16];
	             ASSERTERR($$, "new matrix failed");
		     $$[0] = $1;   $$[1] = $2;   $$[2] = $3;   $$[3] = $4;
                     $$[4] = $5;   $$[5] = $6;   $$[6] = $7;   $$[7] = $8;
                     $$[8] = $9;   $$[9] = $10;  $$[10] = $11; $$[11] = $12;
                     $$[12] = $13; $$[13] = $14; $$[14] = $15; $$[15] = $16;}
		;

t2_field	: TR_filename T_STRING			{s_Texture2_filename($2);}
		|					{s_Texture2_filename("");}
		;

c3_field	: TR_point '[' triples ']'		{$$ = $3;}
		| TR_point  onetriple			{$$ = $2;}
		;

v3_field	: TR_vector '[' triples ']'		{$$ = $3;}
		| TR_vector  onetriple			{$$ = $2;}
		;

tc2_field	: TR_point '[' doubles ']'		{$$ = $3;}
		| TR_point onedouble			{$$ = $2;}
		;

ifs_fields	: ifs_field
		| ifs_fields ifs_field
		;

ifs_field	: TR_coordIndex '[' isingles ']'	{s_CoordIndices($3);}
		| TR_materialIndex '[' isingles ']'	{s_MaterialIndices($3);}
		| TR_normalIndex '[' isingles ']'	{s_NormalIndices($3);}
		| TR_textureCoordIndex '[' isingles ']'	{s_TexCoordIndices($3);}
		;

mat_fields	: mat_fields mat_field
		|
		;

mat_field	: TR_ambientColor  acolor		{s_AmbientColors($2); delete $2;}
		| TR_diffuseColor  dcolor		{s_DiffuseColors($2); delete $2;}
		| TR_emissiveColor ecolor		{s_EmissiveColors($2); delete $2;}
		| TR_specularColor scolor		{s_SpecularColors($2); delete $2;}
		| TR_shininess	   shine		{s_Shininesses($2); delete $2;}
		| TR_transparency  transp		{s_Transparencies($2); delete $2;}
		;

acolor		: '[' triples ']'			{$$ = $2;}
		| onetriple				{$$ = $1;}
		;

dcolor		: '[' triples ']'			{$$ = $2;}
		| onetriple				{$$ = $1;}
		;

ecolor		: '[' triples ']'			{$$ = $2;}
		| onetriple				{$$ = $1;}
		;

scolor		: '[' triples ']'			{$$ = $2;}
		| onetriple				{$$ = $1;}
		;

shine		: '[' fsingles ']'			{$$ = $2;}
		| onefsingle				{$$ = $1;}
		;

transp		: '[' fsingles ']'			{$$ = $2;}
		| onefsingle				{$$ = $1;}
		;

triples		: rtriples			{$$ = $1}
		| rtriples ','			{$$ = $1}
		;

rtriples	: triple			{$$=new vector<Vector>; $$->push_back(*$1); delete $1;}
		| rtriples ',' triple		{$$ = $1; $$->push_back(*$3); delete $3;}
		;

onetriple	: fnum fnum fnum		{$$=new vector<Vector>; $$->push_back(Vector($1, $2, $3));}
		;

triple		: fnum fnum fnum		{$$ = new Vector($1, $2, $3);}
		;

doubles		: rdoubles			{$$ = $1}
		| rdoubles ','			{$$ = $1}
		;

rdoubles	: double			{$$=new vector<Vector>; $$->push_back(*$1); delete $1;}
		| rdoubles ',' double		{$$ = $1; $$->push_back(*$3); delete $3;}
		;

onedouble	: fnum fnum			{$$=new vector<Vector>; $$->push_back(Vector($1, $2, 0));}
		;

double		: fnum fnum			{$$ = new Vector($1, $2, 0);}
		;

isingles	: risingles			{$$ = $1}
		| risingles ','			{$$ = $1}
		| 				{$$ = new vector<int>;}
		;

risingles	: isingle			{$$ = new vector<int>; $$->push_back($1);}
		| risingles ',' isingle		{$$ = $1; $$->push_back($3);}
		;

isingle		: T_INT				{$$ = $1;}

fsingles	: rfsingles			{$$ = $1}
		| rfsingles ','			{$$ = $1}
		;

rfsingles	: fnum				{$$ = new vector<double>; $$->push_back($1);}
		| rfsingles ',' fnum		{$$ = $1; $$->push_back($3);}
		;

onefsingle	: fnum				{$$ = new vector<double>; $$->push_back($1);}
		;

sf_bool		: T_INT				{$$ = (int) ($1) ? 1 : 0;}
                | TR_TRUE			{$$ = 1;}
                | TR_FALSE			{$$ = 0;}
		;

fnum		: T_INT				{$$ = (double) $1;}
                | T_FLOAT			{$$ = $1;}
		;

%%
/* get yylex() from FlexMe.cpp */
#include <Model/FlexMe.cpp>

