ann: ann_aq ann_train ann_run
	# ls -al --color=auto
	echo "Compilation is completed."

ann_aq: ann_aq.c
	gcc -o ann_aq ann_aq.c -lfann -lm

ann_train: ann_train.c
	gcc -o ann_train ann_train.c -lfann

ann_run: ann_run.c
	gcc -o ann_run ann_run.c -lfann	-lm