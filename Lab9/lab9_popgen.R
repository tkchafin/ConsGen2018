#Installing required packages 
install.packages(c("adegenet", "poppr", "vegan", "pegas", "lattice", "mmod", "ggplot2", "reshape2", "plotrix"))

########################## Part 1 ############################
#Understanding our data
#Loading packages for Part 1 
library("poppr")
library("pegas")
library("magrittr")

#load datafile as 'bhs' for Bluehead Sucker 
#This will load your data as a 'genclone' object
bhs <- read.genalex("msat-data_genalex.csv", ploidy=2, geo=F, sep=",")

#You can use the following to see a list of all populations 
#in your genclone object, as well as a summary of the data included:
bhs

#Generate some basic info, such as number of alleles
locus_table(bhs)

#you can also view the same stats for each population:
locus_table(bhs, pop="agr")

#Next, lets take a look at how missing data is distributed:
bhs%>% info_table(plot = TRUE, scale = FALSE)

#This produces a plot with missing data shown per population, 
#and per locus. 

#Answer question 1 in your homework document now. 

#Next, we'll test for Hardy-Weinberg equilibrium for each locus, 
#assuming all sample localities form a signle population,
#using a function hw.test from the package pegas:
hw.test(bhs, B=0)

#In the results, the p-value from the chi-squared test is 
#the columns "Pr(chi^2 >)". Calculate a bonferroni-corrected alpha
#value. 
#Hint: You've applied the HWE test to 16 different loci.

#Answer questions 2-3 in the homework

#Oh no! Lots of loci appear to be out of HWE! 
#This could just be due to sampling across population structure, 
#so let's next test for HWE within each population:
hwe.pops<- seppop(bhs) %>% lapply(hw.test, B=0)
hwe.pops

#Let's make a cool plot which will help us visualize these results:
#First, convert to matrices 
(hwe.mat <- sapply(hwe.pops, "[", i = TRUE, j = 3))

#Now, lets set some things:
alpha <- 0.05 #Replace this with your bonferroni-corrected value 
newmat <- hwe.mat 
newmat[newmat > alpha] <- 1
library("lattice")
levelplot(t(newmat))

#In the resulting plot, values in pink are those which are suspected to NOT be in HWE. 
#Answer question 4. 

#Next, let's check for linkage disequilibrium. 
#First, let's calculate our index of association (r-bar d) over all loci:
ia(bhs, sample=999)

#Here, we're using 999 permutations to give us a p-value. 
#Answer question 5 in the homework. 

#To make sure that we aren't just seeing high association due to a single pair or loci, 
#lets next calculate the index of association pairwise:
ldpair<- bhs %>% pair.ia

#It's possible to see apparent LD due to sampling (Wahlund effect). Let's 
#take a look at if these patterns of LD hold up within populations:
ld.pops<- seppop(bhs) %>% lapply(pair.ia)

#Now, let's standardize the scales, and put them all in a single pdf file
pdf("ld_pops.pdf")
ll<- lapply(ld.pops, function(x) plot(x, limits=c(0.0,0.4)))
dev.off()

#Quickly glance over these results and answer question 6

########################## Part 2 ############################
#Genetic diversity and differentiation
#Now, lets take a look at the diversity within and among
#our populations. 
library("mmod")

#First, lets use the package mmod to calculate some diversity measures
#This will calculate genetic differentiation among pops for each locus.
#We are using this to look at how much divergence is apparent within each locus.
diff<-diff_stats(bhs)

#Let's check out the results. 
#Hs = heterozygosity with pop structure 
#Ht = heterozygosity without pop structure 
#Gst = Nei's Gst
#Gst' = Correct Nei's Gst by dividing by maximum theoretical Gst given heterozygosity
#D = Jost's D
diff

#Lets make a cool plot. Copy and paste all of the below:
library("ggplot2")
library("reshape2")
per.locus <- melt(diff$per.locus, varnames = c("Locus", "Statistic"))
stats     <- c("Hs", "Ht", "Gst", "Gprime_st", "D", "D")
glob      <- data.frame(Statistic = stats, value = diff$global)
ggplot(per.locus, aes(x = Statistic, y = value)) +
  geom_boxplot() +
  geom_point() +
  geom_point(size = rel(3), color = "red", data = glob) +
  ggtitle("Estimates of population differentiation")

#Answer question 7

#You might have noticed a large discrepancy in differentiation using different estimates
#Let's use a different package (diveRsity) to explore further. 
#Here, we will be looking at differentiation AMONG populations, not among loci.
#First, load the package:
library("diveRsity")

#Next, lets look at how Gst and other estimators behave with number of alleles at a locus:
#Note, this is loading a new file. Make sure you're working directory is set
corPlot("bhs_msat.gen") 

#Answer question 8

#Next, we'll calculate divergence PAIRWISE between populations:
pw_diff<-diffCalc("t", pairwise=T)

#Let's plot the pairwise results for Nei's D:
levelplot(pw_diff$pairwise$D, xlab="Pop", ylab="Pop", main="Genetic Distance")

#...and Gst prime
levelplot(pw_diff$pairwise$GGst, xlab="Pop", ylab="Pop", main="Genetic Distance")
par(mfrow=c(1,1))
#Answer question 9 

########################## Part 3 ############################
#Geographic distance and spatial connectivity 
#Don't close R Studio!

#From the pairwise distance results, you should notice that some populations 
#are more closely related, and some are more divergent. To test if this 
#is due to their geographic distance, we'll use a Mantel test. 

#First, we'll need to load our geographic distances and format as matrix.
#I already calculated these in ArcGIS for you... I'm sure there are 
#R-based solutions though. 
dist<-read.table("bhs_dist.csv", header=F, sep=",")
geog<-as.dist(xtabs(dist[,3]~dist[,2]+dist[,1]))
geogmat<-as.matrix(geog)

#Let's save out genetic distance from before as a new variable:
gendist<-pw_diff$pairwise$D
gendist[upper.tri(gendist)] <- t(gendist)[lower.tri(gendist)]

#Next, let's visualize the relationship between genetic and geographic distance:
plot(gendist[lower.tri(gendist)], geogmat[lower.tri(geogmat)], xlab="Genetic Distance", ylab="Geographic Distance", main="IBD")

#Let's fit a linear model:
linreg<-lm(geogmat[lower.tri(geogmat)]~gendist[lower.tri(gendist)])
abline(linreg)

#Now, check Pearson's R for the linear model:
cor(geogmat[lower.tri(geogmat)],gendist[lower.tri(gendist)], method="pearson")

#That's it! You did a mantel test. Congratulations. You can also do it below using
#the package 'vegan':

library("vegan")

#Using the mantel() function in vegan is nice, because it has many other functions, 
#such as using permutations to assess significance. You can also do a stratified Mantel test
#in vegan, if you want to constrain permutations to within strata (e.g. populations).
#Another approach is to do a partial Mantel test, to test for correlations between 
#strata assignments (such as assignments of individuals to species) and genetic distance, 
#conditioning on geographic distance. Pretty neat!
mantel(gendist, geogmat)

#Answer the final questions and then go do something else




########################### Bonus ################################
#cool people will also run one final analysis though. 
#The package diveRsity also implements the methods of Sundqvist et al (2016)
#to try to estimate relative gene flow between populations, given measures 
#of microsatellite divergence, assuming an island model. See the paper for more.

#Here, we'll run their analysis with 100 bootstraps
#This will produce some stuff in est_migration.pdf
pdf("est_migration.pdf")
divMigrate("bhs_msat.gen", boots=100, plot_network=TRUE, para=T)
dev.off()




