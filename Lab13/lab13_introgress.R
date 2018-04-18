setwd("~/Desktop/ConsGen2018/Lab13")

#Install the package
install.packages("introgress", dependencies=T)

#Load the package
library(introgress)

#Load the data input files 
gen.data.sim <- read.csv(file="admix.csv", header=FALSE)
loci.data.sim <- read.csv(file="loci.txt", header=TRUE)
p1.data <- read.csv(file="p1data.csv", header=FALSE)
p2.data <- read.csv(file="p2data.csv", header=FALSE)

#Prep data for analysis
count.matrix <- prepare.data(admix.gen=gen.data.sim, loci.data=loci.data.sim, parental1=p1.data, parental2=p2.data)

#Calulate hybrid index for each individual using maximum-likelihood
#This will take like 10 minutes, sorry
hi.index.sim <- est.h(introgress.data=count.matrix, loci.data=loci.data.sim, fixed=FALSE)

#Plot distribution of hybrid indices
hist(hi.index.sim$h,breaks = 20)

#Calculate interspecific heterozygosity
int.h <- calc.intersp.het(introgress.dat=count.matrix)

#Plot H.index x interspecifit het
triangle.plot(hi.index=hi.index.sim, int.het=int.h, pdf=F)

#Given the placement of samples in this plot, what 'hybrid classes'
#do you think are present in our dataset?

#Calculate a measure of genetic distance for each locus 
deltas<-vector(); for (i in 1:length(loci.data.sim$locus)){deltas[[i]]<-delta(count.matrix$Parental1.allele.freq[i,1:2], count.matrix$Parental2.allele.freq[i,1:2])}

#Esimate genomic clines ONLY for loci with high delta distance between Parent1 and Parent2
#We do this because loci in which P1 and P2 are not differentiated are not very informative
cline_sub1 <- genomic.clines(introgress.data=count.matrix, hi.index=hi.index.sim,loci.data=loci.data.sim,sig.test=TRUE,classification=TRUE, loci.touse=which(deltas>0.8))

#Plot genomic clines
composite.clines(cline_sub1, pdf=F)

#Keep in mind, these may be a little messier looking than I showed earlier, 
#because of the low sample size and subsampling I did to make this run faster
