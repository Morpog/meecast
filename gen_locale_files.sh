#!/bin/bash

ALL_LANGUAGES="ru_RU fi_FI fr_FR en_GB en_US it_IT de_DE es_ES es_MX pl_PL bg_BG lv_LV sk_SK"

xgettext --from-code=UTF-8 --keyword=_ --keyword=Q_ -f po/POTFILES.in -s -C -o po/omweather.pot
cd po
for language in $ALL_LANGUAGES; do
    echo -n "$language: "
    msgmerge -U $language.po omweather.pot
done
cd ..

cp -r po/*.po omweather/po
cp -r po/*.po qtomweather/po

cp -r po/*.pot omweather/po
cp -r po/*.pot qtomweather/po
exit 0