# Modified mango for multi-threads

## What is the bugs

One biggest problem of using Mango pipeline for analyzing ChIA-PET is that they can not support multi threads (cpu):
> Threads option is currently disabled to due to errors. We are working on a solution !!

Their bugs comes from the bowtie mapping step. Two important point for this bugs:
- Mango requires that Fastq files are perfectly matched
- The sam files should still be in the same order as the fastq files

If you use multi-threads for bowtie alignments, the sorts of reads in sam file would be 'out-of-order'

Another point is that:
- SAM files being produced by Mango are not canonical SAM files (i.e., they have no headers)

So we cannot use samtools to sort.

## How to solve

Instead, I modified the raw script to deal with reads (fastq) from SRA or Encode:
- If your reads name of fastq is like:
  ``` shell
  $ head ENCSR981FNA_1.fastq
  @HWI-ST689:335:C7ETCACXX:8:1101:1270:1964 1:N:0:
  GTNGACGCCCACAGGGGGCAGGGTCTCGCCCTTGTAGCGTAAGGTTCCCCCAACATTGGCCACAGAGCCGTTGATGACGACAGCAGTTGGATAAGATATCG
  +
  ```
  Please use the `mango_encode.R` and the same parameter of the original Mango
  
- If your reads name of fastq is like:
  ``` shell
  $ head SRR372741_1.fastq
  @SRR372741.1 B2GA003:1:2:1142:995 length=36
  NCCTTTTCAAATTTACCTAC
  +SRR372741.1 B2GA003:1:2:1142:995 length=36
  #111253566@@CC@C@C@@
  ```
   Please use the `mango_SRA.R` and the same parameter of the original Mango
   
- If others:
  Please modify the `alignBowtie` function (line 17) in `mango_encode.R` or `mango_SRA.R` to make sure you can obtain the right sorted sam file ~

## Install (same with the original Mango, for my personal use)

step1 shell:
``` shell
git clone https://github.com/wangjk321/mango_multithreads_wang.git
mv mango_multithreads_wang mango
R CMD INSTALL --no-multiarch --with-keep.source mango
```

step2 R:
``` R
install.packages('hash')
install.packages('Rcpp')
install.packages('optparse')
install.packages('readr')
```

step3 run the Mango
``` shell
FASTQ=`ls fastq |cut -f 1-7 -d '_'|sort|uniq`

build=hg38
index=/home/Database/bowtie-indexes/UCSC-$build
gt=/home/Database/UCSC/$build/genome_table
mango="Rscript ~/software/mango/mango/mango_encode.R"
mkdir -p mango

for i in $FASTQ
do
        echo $i
        $mango --fastq1 fastq/${i}_1.fastq.gz \
           --fastq2 fastq/${i}_2.fastq.gz \
           --prefix mango/${i} \
           --bowtieref $index \
           --bedtoolsgenome $gt \
           --chromexclude chrM,chrY \
           --stages 1:5 \
           --reportallpairs TRUE \
           --MACS_qvalue 0.05
done
```
