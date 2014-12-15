#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>

typedef int bool;
#define true 1
#define false 0

#define DEBUG_VERTEX_DISTRIBUTION 0
#define DEBUG_JONES_PLASSMANN 0

char * INPUT_PATH = "/home/dsand/ParallelSudoku/MpiVersion/graph_files/";
char * OUTPUT_PATH = "/home/dsand/ParallelSudoku/MpiVersion/graph_files/";
char * ws;//weak/strong indicator
int rank,npes, root =0; 
int V,E;//number of vertices and edges
int chromaticity_upper = -1;//upper bound on the chromaticity of the graph
//chromaticity= minimum number of colors required to color the graph
int *graph;//the symmetric adjacency graph matrix. The index of element at 
//col,row is idx = row*V+col. Similarly the element at index i is at 
//row = i/V,col=i%V
int *weights;//the random weights assigned to each vertex in the beginning
int * colors;//the colors assigned to each vertex


int compare (const void *a, const void *b)
{
  int la = *(const int*) a;
  int lb = *(const int*)b;

  return (la>lb)-(la<lb);
}

//only root will call this function to read the file
//unless DEBUG_VERTEX_DISTRIBUTION is true in which case
//all processes call this function and build their own graph
void read_graph(char *filename)
{
  FILE* file = fopen(filename,"rb");
  if (!file) {
    printf("Unable to open file %s\n",filename);
    return;
  }
  char line[1000];
  char *token;
  int i,j;
  int max_degree=-1;
  int row_idx,col_idx;//uv edge indices
  
  //read file line by line
  j = 0;
  bool graph_initialized = false;
  while (fgets(line,1000,file) != NULL) {
    //read maximum degree of graph
    if (strstr(line,"max degree") != NULL) {
      strtok(line,":");
      token = (char *)strtok(NULL,":");
      max_degree = atoi(token);
      chromaticity_upper = max_degree ;
    }

    if (strstr(line,"chromatic_upper_bound") != NULL) {
      strtok(line,":");
      token = (char *)strtok(NULL,":");
      max_degree = atoi(token);
      chromaticity_upper = max_degree ;
    }

    //tokenize line
    // printf("Full line %s\n",line);
    strtok(line," ");
    //read number of vertices and edges
    if (strcmp(line,"p")==0) {
      token = (char *)strtok(NULL," ");
      token = (char *)strtok(NULL," ");
      V = atoi(token);
      token = (char *)strtok(NULL," ");
      E = atoi(token);
    }
    //read edges into graph matrix
    //1.Initialize graph to all zeros
    if (graph_initialized == false && V>0) {
      graph = (int *) malloc(V*V*sizeof(int));
      memset(graph,0,V*V*sizeof(int));
      graph_initialized = true;
    }
   
    //2.then load edges into it
    if (strcmp(line,"e") == 0) {
      token = (char *)strtok(NULL," ");
      row_idx = atoi(token)-1;//0 based index
      token = (char *)strtok(NULL," ");
      col_idx = atoi(token)-1;//0 based index
      graph[row_idx*V+col_idx]=1;
      graph[col_idx*V+row_idx]=1;//symmetric matrix, unidirectional graph
      j++;
    }
  }
  
  //some files dont have the max_degree. Set it to the max possible chromaticity
  //of a graph which is V if V is odd and V-1 if V is even and would only
  //happen if the graph were complete, i.e. every pair of distinct vertices is
  //connected by a unique edge. See Wikipedia page on complete graphs
  if (max_degree == -1) {
    if (V%2 == 0)
      chromaticity_upper = V-1;
    else
      chromaticity_upper = V;
  }
    
  if (rank == root && j != E && j != E/2) {
      printf("Incorrect edge reading: There are %d edges but read %d.\n",E,j);
  }
  fclose(file);
}

void write_colors(char * filename) 
{
   FILE* file = fopen(filename,"w");
   if (!file) {
     printf("Unable to open file %s\n",filename);
    return;
   }
   int i;
   
   for (i=0;i<V;i++) 
     fprintf(file,"Vertex=%d has color=%d\n",(i+1),colors[i]);

   //write largest color
   qsort(colors,V,sizeof(int),compare);
   fprintf(file,"Largest color was %d\n",colors[V-1]);

   fclose(file);
}

void jones_plassmann(int* range,int * offsets,int * p_graph)
{
  int i,j,k;
  int num_v_per_p,remainder_v_per_p,first_v;
  int * j_colors,*neighbor_colors;//j_colors holds the colors of
  //this vertices process, neighbor_colors the colors of one of these
  int j_weight, num_colors, min_color;
  bool j_weight_is_max;

  //Go through the vertices of this process and compare their weights with
  //neighboring vertices to find which of them are local maxima. Those form
  //the independent set in each iteration
  //The minimum number of colors, i.e. the chromaticity of the graph
  //can not be larger than chromaticity_upper so only iterate that many times
  num_v_per_p = V/npes;
  j_colors = (int *) malloc(range[rank]*sizeof(int));
    memset(j_colors,0,range[rank]*sizeof(int));
    
  for (i=0;i<chromaticity_upper;i++) {    
    //for each vertex in this process
    remainder_v_per_p = (V + rank) % npes;
    //index of the first vertex for process i
    first_v =  num_v_per_p * rank +  remainder_v_per_p * (remainder_v_per_p< rank);
   
     //for each vertex of this process
    for (j=0;j<range[rank];j++) {
	//get the vertex weight
	j_weight = weights[first_v+j];
	j_weight_is_max = true;
	neighbor_colors = (int *) malloc(V*sizeof(int));
	memset(neighbor_colors,0,V*sizeof(int));
	num_colors=0;
	//compare vertex weight to weights of its non-colored neighbors to see
	//if it is a maximum. Also gather the colors of all neighbors of the
	//vertex j that have been colored
	for (k=0;k<V;k++) {
	  //if there is an edge between j vertex and neighbor k vertex
	  if (p_graph[j*V+k]==1) {
	    //if neighbor is colored just add its color to the neighbor_colors
	    if (colors[k] != 0) {
	      neighbor_colors[num_colors++]=colors[k];
	    }
	    //if the weights match, solve conflict by looking at the vertices
	    //ids and taking the vertex with higher id as the max 
	    else if (j_weight< weights[k] || (j_weight==weights[k] && k>j)) {
	      j_weight_is_max = false;
	      break;
	    }
	  }
	}
	//if the vertex weight is a max and vertex hasnt been colored, 
	//color it with the smallest color possible that is not one of
	//neighbor_colors
	if (j_weight_is_max==true && colors[first_v+j]==0) {
	  //find smallest color to assign to the j vertex
	  //that color is either 
	  //a)1 if none of the neighbors is colored or the smallest color
	  //of a neighbor is >1
	  //b)In between a color in the array of neighbors colors if there is
	  //a gap between two of the (sorted) neighbors colors
	  //c) 1 more than the last color in the sorted array of neighbors
	  //colors
	  //sort neighbors colors. 
	  qsort(neighbor_colors,num_colors,sizeof(int),compare);	  
	  if (num_colors==0 || neighbor_colors[0]>1)
	    min_color =1;
	  else {
	    for (k=0;k<V;k++) {
	      if (k<V-1 && (neighbor_colors[k+1]-neighbor_colors[k]>1)) {
		min_color = neighbor_colors[k]+1;
		break;
	      }
	      else {
		min_color = neighbor_colors[num_colors-1]+1;
	      }
	    }
	  }
	  j_colors[j] = min_color;    
#if DEBUG_JONES_PLASSMANN
	  if (i==1) {
	    int m;
	    if (num_colors==0)
	      printf("rank=%d j=%d color=%d\n",rank,j,min_color);
	    for (m=0;m<num_colors;m++) {
	      printf("rank=%d j=%d color=%d neighbors colors %d\n",rank,j,min_color,neighbor_colors[m]);
	    }
	  }
#endif
	}
	free(neighbor_colors);
    }
    //each process sends the colors of its vertices to root
    MPI_Gatherv(j_colors,range[rank],MPI_INT,colors,range,offsets,MPI_INT,root,MPI_COMM_WORLD);
    //root synchronizes colors on all processes
    MPI_Bcast(colors,V,MPI_INT,root,MPI_COMM_WORLD);
#if DEBUG_JONES_PLASSMANN
    if (i==1) {
      int p;
      for (p=0;p<range[rank];p++){      
	printf("Checking copy from j_colors to colors:rank=%d vertex=%d j_color=%d colors=%d\n",rank,offsets[rank]+p,j_colors[offsets[rank]+p],colors[offsets[rank]+p]);
      }
    }
#endif
  }
  free(j_colors);
}

int main(int argc,char** argv)
{
  char * input_filename, *output_filename;
  int num_v_per_p, remainder_v_per_p; 
  int first_v, last_v,*range;//ids of the first and last vertices in
  //a given processor
  int i,j,k;
  int * p_graph,*p_graph_size,*offsets;//graph with edges corresponding to the
  //vertices in this process and their size. Note that if say process p
  //has vertices 0 and 1, then p_graph will be a 2xV matrix, so it has
  //all the edges between 1,2 and all vertices
  //offsets is the index in graph where to start copying to p_graph
  int *vertex_offsets;
  double start_time,end_time,runtime,largest_runtime;

  //Initialize
  MPI_Init(&argc,&argv); 
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &npes);

  if (argc != 3) { 
    if (rank == root)
      printf("Usage: mpirun/mpiexec -np xx ./graphcoloring input_filename output_filename\n");

    MPI_Finalize();
    return -1;
  }

  //Read whether we are doing strong/weak scaling and the input and output
  //filenames
  /*ws = argv[1];

  if (strcmp(ws,"strong")!=0 && strcmp(ws,"weak")!=0) {
    if (rank == root)
      printf("Invalid option. Please enter strong or weak for the second command line option.\n");

    MPI_Finalize();
    return -1;
    } 

    if (strcmp(ws,"strong") == 0) {
    psize = n/npes;
  }   
  if (strcmp(ws,"weak") == 0) {
    psize = n;
    }*/

  input_filename = malloc(900);
  strcpy(input_filename,INPUT_PATH);
  strcat(input_filename,argv[1]);

  output_filename = malloc(900);
  strcpy(output_filename,OUTPUT_PATH);
  strcat(output_filename,argv[2]);
  // printf("%s %s\n",input_filename,output_filename);
  
  //root reads file
#if DEBUG_VERTEX_DISTRIBUTION
  //every process reads the file and loads graph 
  read_graph(input_filename);
  if (rank == root) {
    printf("V=%d E=%d max_degree=%d\n",V,E,chromaticity_upper);
    fflush(stdout);
  }
#else
  //only root reads the file and loads the full graph
  if (rank == root) {
    read_graph(input_filename);
    printf("V=%d E=%d Chromaticity Upper Bound=%d\n",V,E,chromaticity_upper);
    printf("Root finished reading the graph from file.\n");
    fflush(stdout);
  }
  //root broadcasts E,V and chromaticity_upper to all other processes
  MPI_Bcast(&V,1,MPI_INT,root,MPI_COMM_WORLD);
  MPI_Bcast(&E,1,MPI_INT,root,MPI_COMM_WORLD);
  MPI_Bcast(&chromaticity_upper,1,MPI_INT,root,MPI_COMM_WORLD);
#endif
  //Distribute the number of vertices as uniformly as possible among the
  //processors: that means (V+rank)/npes vertices per process (integer
  //division) with npes being the processor id 0..np-1
  p_graph_size = (int *)malloc(npes * sizeof(int));
  offsets = (int *)malloc(npes * sizeof(int));
  vertex_offsets = (int *)malloc(npes * sizeof(int));
  range = (int *)malloc(npes * sizeof(int));
  num_v_per_p = V/npes;
  for (i=0;i<npes;i++) {  
    remainder_v_per_p = (V + i) % npes;
    //index of the first vertex for process i
    first_v =  num_v_per_p * i +  remainder_v_per_p * (remainder_v_per_p< i); 
    //index of the last vertex for process i
    last_v = (i + 1)* num_v_per_p + (remainder_v_per_p+1) * (remainder_v_per_p<i)-1;
    range[i] = last_v-first_v + 1;
    p_graph_size[i] = range[i]*V;
    
    offsets[0] = 0;
    vertex_offsets[0]=0;
    if (i>0) {
      offsets[i] = offsets[i-1] + p_graph_size[i-1];
      vertex_offsets[i] = vertex_offsets[i-1] + range[i-1];
    }
#if DEBUG_VERTEX_DISTRIBUTION    
    printf("V=%d rank=%d first=%d last=%d range=%d ideal_range=%d\n",V,i,first_v,last_v,range[i],(V+i)/npes);
#endif
  }
 
  //root sends portion of the graph array corresponding to vertices of the 
  //process to each process
  p_graph = (int *) malloc(p_graph_size[rank]*sizeof(int));
 
  MPI_Scatterv(graph,p_graph_size,offsets,MPI_INT,p_graph,p_graph_size[rank],MPI_INT,root,MPI_COMM_WORLD);

#if DEBUG_VERTEX_DISTRIBUTION
  //check whether p_graph and graph match in the corresponding positions
  for (i=0;i<range[rank];i++) {
    for (j=0;j<V;j++) {
      if (p_graph[i*V+j] != graph[offsets[rank]+i*V+j])
	printf("Incorrect sub graph assignment in p_graph process %d at row %d col %d\n",rank,i,j);
    }
  }
#endif
  //measure runtime in each process
  start_time = MPI_Wtime();
  weights = (int *) malloc(V*sizeof(int));
  //Root generates random weights which are a permutation of the vertices
  if (rank == root) {
    for (i=0;i<V;i++) {
      weights[i] = rand()%(V*1000);
    }
  }
  //weights are sent to every process
  MPI_Bcast(weights,V,MPI_INT,root,MPI_COMM_WORLD);

  //initialize colors to 0
  colors = (int *)malloc(V*sizeof(int));
  memset(colors,0,V*sizeof(int));

  //Jones-Plassman algorithm
  jones_plassmann(range,vertex_offsets,p_graph);
  
  end_time = MPI_Wtime();
  runtime = end_time-start_time;

  //find the largest runtime (most likely it will be root's runtime since root
  //does a few extra things like generating weights, gathering colors and
  //synchronizing them)
  MPI_Allreduce(&runtime,&largest_runtime,1,MPI_DOUBLE,MPI_MAX,MPI_COMM_WORLD);

  printf("Max runtime was %f\n",largest_runtime);

  //find out how many vertices are uncolered
  int num_uncolored =0;
  for (i=0;i<V;i++) {
    if (colors[i]==0)
      num_uncolored++;
  }
  if (num_uncolored>0)
    printf("Not all vertices have been colored");

  //root writes colors to file
  if (rank == root){
    write_colors(output_filename);    
  }
  
  free(input_filename);
  free(output_filename);
  free(p_graph);
  free(range);
  free(p_graph_size);
  free(offsets);
  free(graph);
  free(weights);
  free(colors);
  //Finalize
  MPI_Finalize();
  return 0;
}
