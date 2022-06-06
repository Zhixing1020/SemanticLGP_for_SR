#!/bin/bash
#SBATCH -a 1-30
#SBATCH --cpus-per-task=1
#SBATCH --mem-per-cpu=15G
#SBATCH --partition=parallel
#SBATCH --time=0-20:00:00
#SBATCH -o /***/SLGP_exp/SLGP-ERC/rap_output/%x-%A-%a.o.txt
#SBATCH -e /***/SLGP_exp/SLGP-ERC/rap_output/%x-%A-%a.e.txt
#SBATCH --mail-type=BEGIN,END,FAIL
#SBATCH --mail-user=me@email.com

GRID_PATH="/***/SLGP_exp/SLGP-ERC/code"

PROBLEM=$1

if [ ! -d $GRID_PATH/$PROBLEM ]; then
  mkdir $GRID_PATH/$PROBLEM
fi

$GRID_PATH/SLGP-ERC $1 $(($SLURM_ARRAY_TASK_ID-1)) $GRID_PATH/$PROBLEM