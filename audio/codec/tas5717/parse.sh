#!/bin/bash
clear
if test -z "$1"; then
	echo -e "\n\n\t\t!!!You should give a file to parse!!!!\n\n"
	exit
else
	if test -e "$1"; then
		echo ">>>>> Parse file ${1} <<<<<"
	else
		echo -e "\n\n\t\t!!!You should give a exist file to parse!!!!\n\n"
		exit
	fi
fi

INPUT_FILE=$1
TMP_FILE=/tmp/tmp_${INPUT_FILE}

# Register List Of TAS5717
CH1_EQ="X26 X27 X28 X29 X2A X2B X2C X2D X2E X2F"
CH2_EQ="X30 X31 X32 X33 X34 X35 X36 X37 X38 X39"
DRC1="X3B X3C X40"
DRC2="X3E X3F X43"

#EQ of Channel 1
echo -e "\nEQ_Ch1+EQ_Ch2:\n"
for reg in ${CH1_EQ}
do
	grep -rw --color "^${reg}" ${INPUT_FILE} > ${TMP_FILE}
	val_00=`cat  ${TMP_FILE} | awk -F ' ' {'print $2'}`
	val_01=`cat  ${TMP_FILE} | awk -F ' ' {'print $3'}`
	val_02=`cat  ${TMP_FILE} | awk -F ' ' {'print $4'}`
	val_03=`cat  ${TMP_FILE} | awk -F ' ' {'print $5'}`
	val_04=`cat  ${TMP_FILE} | awk -F ' ' {'print $6'}`
	val_05=`cat  ${TMP_FILE} | awk -F ' ' {'print $7'}`
	val_06=`cat  ${TMP_FILE} | awk -F ' ' {'print $8'}`
	val_07=`cat  ${TMP_FILE} | awk -F ' ' {'print $9'}`
	val_08=`cat  ${TMP_FILE} | awk -F ' ' {'print $10'}`
	val_09=`cat  ${TMP_FILE} | awk -F ' ' {'print $11'}`
	val_10=`cat  ${TMP_FILE} | awk -F ' ' {'print $12'}`
	val_11=`cat  ${TMP_FILE} | awk -F ' ' {'print $13'}`
	val_12=`cat  ${TMP_FILE} | awk -F ' ' {'print $14'}`
	val_13=`cat  ${TMP_FILE} | awk -F ' ' {'print $15'}`
	val_14=`cat  ${TMP_FILE} | awk -F ' ' {'print $16'}` 
	val_15=`cat  ${TMP_FILE} | awk -F ' ' {'print $17'}`
	val_16=`cat  ${TMP_FILE} | awk -F ' ' {'print $18'}`
	val_17=`cat  ${TMP_FILE} | awk -F ' ' {'print $19'}`
	val_18=`cat  ${TMP_FILE} | awk -F ' ' {'print $20'}`
	val_19=`cat  ${TMP_FILE} | awk -F ' ' {'print $21'}`
	echo "                0x${val_00} 0x${val_01} 0x${val_02} 0x${val_03} 0x${val_04} 0x${val_05} 0x${val_06} 0x${val_07} 0x${val_08} 0x${val_09} 0x${val_10} 0x${val_11} 0x${val_12} 0x${val_13} 0x${val_14} 0x${val_15} 0x${val_16} 0x${val_17} 0x${val_18} 0x${val_19} /* 0${reg}---ch1 */"
done

#EQ of Channel 2
#echo -e "\nEQ_Ch2:\n"
for reg in ${CH2_EQ}
do
	grep -rw --color "^${reg}" ${INPUT_FILE} > ${TMP_FILE}
	val_00=`cat  ${TMP_FILE} | awk -F ' ' {'print $2'}`
	val_01=`cat  ${TMP_FILE} | awk -F ' ' {'print $3'}`
	val_02=`cat  ${TMP_FILE} | awk -F ' ' {'print $4'}`
	val_03=`cat  ${TMP_FILE} | awk -F ' ' {'print $5'}`
	val_04=`cat  ${TMP_FILE} | awk -F ' ' {'print $6'}`
	val_05=`cat  ${TMP_FILE} | awk -F ' ' {'print $7'}`
	val_06=`cat  ${TMP_FILE} | awk -F ' ' {'print $8'}`
	val_07=`cat  ${TMP_FILE} | awk -F ' ' {'print $9'}`
	val_08=`cat  ${TMP_FILE} | awk -F ' ' {'print $10'}`
	val_09=`cat  ${TMP_FILE} | awk -F ' ' {'print $11'}`
	val_10=`cat  ${TMP_FILE} | awk -F ' ' {'print $12'}`
	val_11=`cat  ${TMP_FILE} | awk -F ' ' {'print $13'}`
	val_12=`cat  ${TMP_FILE} | awk -F ' ' {'print $14'}`
	val_13=`cat  ${TMP_FILE} | awk -F ' ' {'print $15'}`
	val_14=`cat  ${TMP_FILE} | awk -F ' ' {'print $16'}` 
	val_15=`cat  ${TMP_FILE} | awk -F ' ' {'print $17'}`
	val_16=`cat  ${TMP_FILE} | awk -F ' ' {'print $18'}`
	val_17=`cat  ${TMP_FILE} | awk -F ' ' {'print $19'}`
	val_18=`cat  ${TMP_FILE} | awk -F ' ' {'print $20'}`
	val_19=`cat  ${TMP_FILE} | awk -F ' ' {'print $21'}`
	echo "                0x${val_00} 0x${val_01} 0x${val_02} 0x${val_03} 0x${val_04} 0x${val_05} 0x${val_06} 0x${val_07} 0x${val_08} 0x${val_09} 0x${val_10} 0x${val_11} 0x${val_12} 0x${val_13} 0x${val_14} 0x${val_15} 0x${val_16} 0x${val_17} 0x${val_18} 0x${val_19} /* 0${reg}---ch2 */"
done

#DRC1
echo -e "\nDRC1:\n"
for reg in ${DRC1}
do
	grep -rw --color "^${reg}" ${INPUT_FILE} > ${TMP_FILE}
	val_00=`cat  ${TMP_FILE} | awk -F ' ' {'print $2'}`
	val_01=`cat  ${TMP_FILE} | awk -F ' ' {'print $3'}`
	val_02=`cat  ${TMP_FILE} | awk -F ' ' {'print $4'}`
	val_03=`cat  ${TMP_FILE} | awk -F ' ' {'print $5'}`
	val_04=`cat  ${TMP_FILE} | awk -F ' ' {'print $6'}`
	val_05=`cat  ${TMP_FILE} | awk -F ' ' {'print $7'}`
	val_06=`cat  ${TMP_FILE} | awk -F ' ' {'print $8'}`
	val_07=`cat  ${TMP_FILE} | awk -F ' ' {'print $9'}`
	echo "                0x${val_00} 0x${val_01} 0x${val_02} 0x${val_03} 0x${val_04} 0x${val_05} 0x${val_06} 0x${val_07} /* 0${reg}---drc1 */"
done

#DRC2
echo -e "\nDRC2:\n"
for reg in ${DRC2}
do
	grep -rw --color "^${reg}" ${INPUT_FILE} > ${TMP_FILE}
	val_00=`cat  ${TMP_FILE} | awk -F ' ' {'print $2'}`
	val_01=`cat  ${TMP_FILE} | awk -F ' ' {'print $3'}`
	val_02=`cat  ${TMP_FILE} | awk -F ' ' {'print $4'}`
	val_03=`cat  ${TMP_FILE} | awk -F ' ' {'print $5'}`
	val_04=`cat  ${TMP_FILE} | awk -F ' ' {'print $6'}`
	val_05=`cat  ${TMP_FILE} | awk -F ' ' {'print $7'}`
	val_06=`cat  ${TMP_FILE} | awk -F ' ' {'print $8'}`
	val_07=`cat  ${TMP_FILE} | awk -F ' ' {'print $9'}`
	echo "                0x${val_00} 0x${val_01} 0x${val_02} 0x${val_03} 0x${val_04} 0x${val_05} 0x${val_06} 0x${val_07} /* 0${reg}---drc2 */"
done

rm -rf ${TMP_FILE}
