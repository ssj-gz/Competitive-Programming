#! /bin/bash

function usage() {
    echo "Expected: directory containing manim from git @9174ec335c13924a52885779e7e2216811f91117"
}

if [ "$#" != "1" ]; then
    usage
    exit 1
fi

MANIM_DIR=$1
MANIM_EXECUTABLE="${MANIM_DIR}/manim.py"
if [ ! -f "${MANIM_EXECUTABLE}" ]; then
    echo "Provided manim directory '${MANIM_DIR}' should contain a 'manim.py' executable - aborting"
    exit 1
fi

export PYTHONPATH=.:"${MANIM_DIR}"
EDITORIAL_MANIMS_BASENAME="editorial-animations" # ${EDITORIAL_MANIMS_BASENAME}.py is the "entry point" manim needs to generate our animations.

FIND_SCENE_NAMES_SCRIPT=`cat <<EOF
#! /usr/bin/python3

editoral_animations_module = __import__("${EDITORIAL_MANIMS_BASENAME}")
#from manimlib import extract_scene
from manimlib.extract_scene import get_scene_classes_from_module

for sceneClass in get_scene_classes_from_module(editoral_animations_module):
    print (sceneClass.__name__)
EOF`

rm -f Makefile
echo "export PYTHONPATH := ${PYTHONPATH}" >> Makefile
readarray -t ALL_SCENE_NAMES < <(python -c "${FIND_SCENE_NAMES_SCRIPT}")
declare -p ALL_SCENE_NAMES
echo "ALL_SCENE_NAMES: ${ALL_SCENE_NAMES[*]}"
ALL_MAKE_TARGETS=""
MAKE_BODY=""
for SCENE_NAME in ${ALL_SCENE_NAMES[*]}; do
    MAKE_TARGET="./media/videos/editorial-animations/1440p60/${SCENE_NAME}.mp4" 
    MAKE_BODY="${MAKE_BODY}\n${MAKE_TARGET}:\n"
    MAKE_BODY="${MAKE_BODY}\t${MANIM_EXECUTABLE} ${EDITORIAL_MANIMS_BASENAME}.py ${SCENE_NAME}\n" >> Makefile
    ALL_MAKE_TARGETS="${ALL_MAKE_TARGETS} ${MAKE_TARGET}"
done
echo -e "all: ${ALL_MAKE_TARGETS}\n\techo 'Making all'" >> Makefile
echo -e "${MAKE_BODY}" >> Makefile
echo "Makefile generated.  Please run 'make'"
