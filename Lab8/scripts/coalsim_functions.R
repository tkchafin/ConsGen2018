library(ape)
library(coalescentMCMC)
library(adephylo)
library(coala)
library(scrm)


#FUNCTIONS -- LOAD THESE FIRST
##########################################
#Simulate singe population with bottleneck
##########################################
#alpha is growth rate. POsitive alpha = forward time growth, negative alpha = forward time shrinking
single_pop_Ne <- function(size, loci, alpha){
  model <- coal_model(size, loci) +
    sumstat_trees() +
    feat_ignore_singletons(locus_group="all") +
    feat_size_change(alpha, time=.2, locus_group="all")
    feat_size_change(1.0, time=.4, locus_group="all")
    feat_mutation(10) +
    sumstat_sfs()
  stats <- simulate(model)
  #Get coalescent times for set of gene trees (in coalescent units)
  trees <- lapply(stats$trees, function(x) read.tree(text=x))
  return(trees)
}

##########################################
###Single pop time to MRCA plots
##########################################
single_pop_mrca_plot<-function(trees, title){
  if(missing(title)){
    title = "Coalescent time to MRCA"
  }
  pl_start = 0
  mrca_times <- list()
  total <- vector()
  i <- 1
  for (tree in trees){
    mrca_times[[i]] <- branching.times(tree)
    i <- i+1
    total<- c(total, branching.times(tree))
  }
  plot(density(total), col="red", lwd=2, main=title, xlab="Coalescent Units", xlim=c(0,.5))
  for (loc in mrca_times){
    lines(density(loc), lwd=.2)
  }
  lines(density(total),col="red",lwd=2)
}


########################################################
#Pairwise coalescent times for two pops
#pop1 and pop2 are vectors of node labels for those pops
########################################################
two_pops_cophenetic <- function(pop1, pop2, trees, title) {
  if(missing(title)){
    title = "Pairwise Coalescent Times"
  }
  pw_matrices <- lapply(trees, function(x) cophenetic(x))
  pl_start = 0
  for (i in pop1){
    for (j in pop2){
      pw <- sapply(pw_matrices, function(x) x[toString(i),toString(j)])
      if(pl_start==0){
        plot(density(pw), main=title, xlab="Coalescent Units")
        pl_start = 1
      }else{
        lines(density(pw))
      }
    }
  }
}

######################################################
#Coala call to simulate two pops with gene flow
######################################################
#Inputs: size=number of inds to sample (from each), num Loci, mig rate, start+end of mig
two_pops_geneflow<- function(size,loci, m, t){
  model <- coal_model(c(size,size), loci) +
    sumstat_trees() +
    feat_ignore_singletons(locus_group="all") +
    feat_migration(m, pop_from=1, pop_to=2, time=t, locus_group="all") +
    feat_mutation(10, fixed_number=F, locus_group="all") +
    sumstat_sfs()
  stats <- simulate(model)
  #Get coalescent times for set of gene trees (in coalescent units)
  trees <- lapply(stats$trees, function(x) read.tree(text=x))
  coal_dist <- sapply(trees, function(x) distRoot(x,tips=c(1,10)))
  return(trees)
}
