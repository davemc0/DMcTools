/*** FlexMe.l ***/
%{
#include <Model/LoadVRML.h>

char *stripquotes(void);
char *stripunderscore(void);
int my_linecount = 2; /* Since we read the #VRML 1.0 line outside. */

/* also need to change value in fscanf */
#define LINE_MAXLEN 5120

/* the input file */
extern FILE *InFile;

/* New YY_INPUT to save input line (for error messages), and convert from
 * DOS format (\r\n) to Unix format (\n) as needed
 */
char my_linetext[LINE_MAXLEN];
#undef YY_INPUT
#define YY_INPUT(buf,result,max_size)\
{\
  char *c;\
  if(max_size < LINE_MAXLEN)\
    {\
      fprintf(stderr,"** YY_INPUT: max_size err\n");\
      exit(1);\
    }\
  result = 0;\
  buf[0] = '\0';\
  c=fgets(buf, LINE_MAXLEN, InFile);\
  result=strlen(buf);\
  if(c==0)\
    {\
      result = YY_NULL;\
    }\
  else\
    {\
      if ((result>1) && (buf[result-2]=='\r'))\
	{\
	  buf[result-2]='\n'; buf[result--]='\0';\
	}\
      else\
	{\
	}\
      strcpy(my_linetext,buf);\
    }\
}
%}
letter	[a-zA-Z\-\(\)]
digit	[0-9]
uint    {digit}+
%%
[ \t]+	{ /* delete white space */}
[\n]    { /* increment line count */ my_linecount++;}

#[^\n]*\n      {/* comment fprintf(stderr, "Comment: %s", my_linetext); */ my_linecount++;}

DEF			return(TR_DEF);
Separator		return(TR_Separator);
Group			return(TR_Separator);
TransformSeparator	return(TR_Separator);
WWWAnchor		return(TR_Separator);
name			  return(TR_name);
map			  return(TR_map);
NONE			    return(TR_NONE);
Info			return(TR_Info);
string			  return(TR_string);
PerspectiveCamera	return(TR_PerspectiveCamera);
position		  return(TR_position);
orientation		  return(TR_orientation);
focalDistance		  return(TR_focalDistance);
heightAngle		  return(TR_heightAngle);
MaterialBinding		return(TR_MaterialBinding);
NormalBinding		return(TR_NormalBinding);
value			  return(TR_value);
OVERALL			  return(TR_OVERALL);
PER_FACE		  return(TR_PER_FACE);
PER_FACE_INDEXED	  return(TR_PER_FACE_INDEXED);
PER_VERTEX		  return(TR_PER_VERTEX);
PER_VERTEX_INDEXED	  return(TR_PER_VERTEX_INDEXED);
ShapeHints		return(TR_ShapeHints);
vertexOrdering		  return(TR_vertexOrdering);
COUNTERCLOCKWISE	  return(TR_COUNTERCLOCKWISE);
CLOCKWISE		  return(TR_CLOCKWISE);
shapeType		  return(TR_shapeType);
SOLID			  return(TR_SOLID);
UNKNOWN_SHAPE_TYPE	  return(TR_UNKNOWN_SHAPE_TYPE);
faceType		  return(TR_faceType);
CONVEX			  return(TR_CONVEX);
creaseAngle		  return(TR_creaseAngle);
MatrixTransform		  return(TR_MatrixTransform);
matrix			  return(TR_matrix);
Scale			return(TR_Scale);
scaleFactor		  return(TR_scaleFactor);
Rotation		return(TR_Rotation);
rotation		  return(TR_rotation);
Translation		return(TR_Translation);
translation		  return(TR_translation);
Texture2Transform	return(TR_Texture2Transform);
center			  return(TR_center);
renderCulling		return(TR_renderCulling);
ON			  return(TR_ON);
OFF			  return(TR_OFF);
AUTO			  return(TR_AUTO);
TRUE			  return(TR_TRUE);
FALSE			  return(TR_FALSE);
Texture2		return(TR_Texture2);
filename		return(TR_filename);
Normal			return(TR_Normal);
vector			return(TR_vector);
Coordinate3		return(TR_Coordinate3);
point			return(TR_point);
Material		return(TR_Material);
ambientColor		  return(TR_ambientColor);
specularColor		  return(TR_specularColor);
emissiveColor		  return(TR_emissiveColor);
diffuseColor		  return(TR_diffuseColor);
shininess		  return(TR_shininess);
transparency		  return(TR_transparency);
TextureCoordinate2	return(TR_TextureCoordinate2);
IndexedFaceSet		return(TR_IndexedFaceSet);
coordIndex		  return(TR_coordIndex);
materialIndex		  return(TR_materialIndex);
normalIndex		  return(TR_normalIndex);
textureCoordIndex	  return(TR_textureCoordIndex);
Cylinder		return(TR_Cylinder);
radius			  return(TR_radius);
Cube			return(TR_Cube);
width			  return(TR_width);
height			  return(TR_height);
depth			  return(TR_depth);
DirectionalLight	return(TR_DirectionalLight);
PointLight		return(TR_PointLight);
SpotLight		return(TR_SpotLight);
color			  return(TR_color);
intensity		  return(TR_intensity);
direction		  return(TR_direction);
location		  return(TR_location);
on			  return(TR_on);
dropOffRate		  return(TR_dropOffRate);
cutOffAngle		  return(TR_cutOffAngle);

RotationKey_ktx_com	return(TR_RotationKey_ktx_com);
AnimationFrames_ktx_com	return(TR_AnimationFrames_ktx_com);
ScaleKey_ktx_com	return(TR_ScaleKey_ktx_com);
scale			  return(TR_scale);
fields			  return(TR_fields);
frame			  return(TR_frame);
AnimationStyle_ktx_com	return(TR_AnimationStyle_ktx_com);
splineUse		  return(TR_splineUse);
pivotOffset		  return(TR_pivotOffset);
PositionKey_ktx_com	return(TR_PositionKey_ktx_com);
AimTarget_ktx_com	return(TR_AimTarget_ktx_com);
aimer			  return(TR_aimer);
length			  return(TR_length);
rate			  return(TR_rate);

("+"|"-")?{uint}			{ yylval.ival = VRML_ATOI(yytext);
                                          return(T_INT); /*integer literal */}

("+"|"-")?(({uint}?"."{uint})|({uint}"e"("+"|"-")?{uint})|({uint}"."{uint}"e"("+"|"-")?{uint})|({uint}".")) {
                                          /* Follow doesn't work on cystine:*/
                                          /* yylval.fval=VRML_ATOF(yytext); */
                                          sscanf(yytext,"%lf", &(yylval.fval));
					  return(T_FLOAT); /*float literal */}

\"([^\"\n]|\"\")*\"			{ yylval.sval = stripquotes(); 
					  return(T_STRING); }

\"([^\"\n]|\"\")*\n			{ yylval.sval = stripquotes(); 
					  fprintf(stderr,"err:Run-on string\n");
					  return(T_STRING); }

"{"|"}"|"["|"]"|","			{ /* brackets, comma */
					  return(yytext[0]); }

{letter}({letter}|{digit}|"_")*		{ yylval.sval = strdup(yytext);

	/* fprintf(stderr, "%d Got a T_STRING: '%s'\n", yyleng, yylval.sval); */
	/* single-word identifier */ return(T_STRING);}

.	{ fprintf(stderr, "Syntax Error (input line %i)\n", my_linecount);
          return(T_ERROR); }

%%

/* Strip unwanted quotes from string in yytext,
   reduce "" to ".
   Shamelessly cribbed from Crafting a compiler with C, pg. 67
   Modified to produce \" instead of ", and \\ instead of \
*/
char *stripquotes(void)
{
  char *result;
  int frompos, topos = 0;

  result = (char *) malloc(yyleng*2); /* big enough for all backslash */
  if (result == NULL) {fprintf(stderr, "stripquotes: malloc() failed\n");
                       exit(1);}
  for (frompos = 1; frompos < (yyleng-1); frompos++) {
    if (yytext[frompos] == '\\') {
      result[topos++] = '\\';
    } else if ((yytext[frompos] == '"') && (yytext[frompos+1] == '"')) {
      result[topos++] = '\\'; frompos++;
    }
    result[topos++] = yytext[frompos];
  }
  result[topos] = '\0';
  return(result);
}

/*
 * return pointer to yytext
 * called by bison code
 */
char *tokentext(void) {
  return(yytext);
}
