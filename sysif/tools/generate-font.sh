#!/bin/bash

# ImageMagick
command -v convert >/dev/null 2>&1 || { echo >&2 "imagemagick n'est pas installe, il est alors impossible de generer la police demandee. Essayer 'sudo apt-get install imagemagick'"; exit 1; }

# Graphics Magick
command -v gm >/dev/null 2>&1 || { echo >&2 "graphics magick n'est pas installe, il est alors impossible de generer la police demandee. Essayer 'sudo apt-get install graphicsmagick'"; exit 1; }

FONT_SIZE=16
FONT_NAME=Courier-Regular

ASCII_START=32
ASCII_END=126

TMP_DIR_NAME=tmp-font
TMP_DIR_PATH=`pwd`

FONT_FILE_NAME=font_spec.h
FONT_FILES_PATH=`pwd`

mkdir tmp-font

if [ -d "${TMP_DIR_NAME}" ]
then
       	echo "Creation du repertoire temporaire ${TMP_DIR_PATH}/${TMP_DIR_NAME}"
else
       	echo "Erreur lors de la creation du repertoire temporaire ${TMP_DIR_PATH}/${TMP_DIR_NAME}"
	exit 1
fi

echo "#ifndef FONT_SPEC_H_
#define FONT_SPEC_H_
" > ${FONT_FILES_PATH}/${FONT_FILE_NAME}

for ((i = $ASCII_START; i <= $ASCII_END; i++)); do
	n=`printf "\x$(printf %x $i)"`
  convert +antialias -font $FONT_NAME -pointsize $FONT_SIZE label:$n ${TMP_DIR_PATH}/${TMP_DIR_NAME}/$i.xbm && \
  #gm convert +antialias -font $FONT_NAME -pointsize $FONT_SIZE label:$n ${TMP_DIR_PATH}/${TMP_DIR_NAME}/$i.xbm && \
	cat ${TMP_DIR_PATH}/${TMP_DIR_NAME}/$i.xbm | sed "s/#define /#define char_font_/" | sed "s/static char /static char char_font_/" >> ${FONT_FILES_PATH}/${FONT_FILE_NAME}
  if [[ $? != 0 ]]
  then 
      gm convert +antialias -pointsize $FONT_SIZE label:$n ${TMP_DIR_PATH}/${TMP_DIR_NAME}/$i.xbm && \
      cat ${TMP_DIR_PATH}/${TMP_DIR_NAME}/$i.xbm | sed "s/#define /#define char_font_/" | sed "s/static char /static char char_font_/" >> ${FONT_FILES_PATH}/${FONT_FILE_NAME}
  fi

done

echo "
#endif /* FONT_SPEC_H_ */" >> ${FONT_FILES_PATH}/${FONT_FILE_NAME}

echo "Creation du fichier ${FONT_FILES_PATH}/${FONT_FILE_NAME}"

read -p "mv font_spec.h ? (Y/n) : " -r
echo ""
if [[ $REPLY =~ ^[Yy]$ ]];
then
    mv -f ${FONT_FILE_NAME} ../src/
fi

read -p "Supprimer le dossier temporaire ? (Y/n) : " -r
echo ""
if [[ ! $REPLY =~ ^[Yy]$ ]];
then
    exit 1
fi



rm -f ${TMP_DIR_PATH}/${TMP_DIR_NAME}/*.xbm
rmdir ${TMP_DIR_PATH}/${TMP_DIR_NAME}
