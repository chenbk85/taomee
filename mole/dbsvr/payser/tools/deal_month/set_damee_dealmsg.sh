if [ $# -ne 1  ]  ; then
	echo "request: $0 filename"
	exit
fi
set_damee_dealmsg $1  | tee $1_err
