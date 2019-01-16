#include "libpolygon/two_complex.h"

Simplex::Simplex() 
{
    int i;
    char tmp[10];
    string tmp2;

    color = "#";
    for ( i = 0; i < 12; i++ ) {
	sprintf(tmp,"%lx",my_random() %16);
	tmp2 = tmp;
	color += tmp2;
    }

};

int Simplex::id()
{
    return ID;
}


void Simplex::Print(ostream& out) 
{
//    fprintf(out_f,"%c%d ",tag,id());
    out << tag << id(); 
};


//    
//    simplex(color_t c) {
//	id = 0;
//	visit_id = 0;
//	color = c;

Vertex::Vertex() :Simplex()
{
    tag = 'V';
    order = 0;
    euclidean = UNDEFINED;
	
};


bool Vertex::deleted()
{

   if ( order == 0 ) 
     return(true);

   return(false);
}

void Vertex::Delete()
{
    order = 0;
}


void Vertex::Check() 
{

    OEdgePtrIter i,j;
    
    if( deleted())
	return;

    if( (int)out_edges.size() != order ) {
	std::cout << "V" << ID <<":" << "out_edges.size = " << out_edges.size() << " order = " << order << endl;
	ERR_RET("vertex_check:vertex order does not match");
    }
    Point q = Point(0,0);

    for( i = out_edges.begin(); i != out_edges.end(); i++ ) {
	if( (*i)->head() != this ) {
	    ERR_RET("vertex check: vertex not outgoing");
	}
//	/* assume triangles */
	q += (*i)->next_edge()->vec_cx();

//	q = q -(*i)->prev_edge()->vecQ() - (*i)->vecQ();

	j = next_vert_edge(i);

	if( ! CCW ( (*i)->vec_cx(), (*j)->vec_cx())) {
	    ERR_RET("vertex check: edges not CCW");
	}
    }
    //    std::cout << "V" << id() << ": Total vertex vec: " << q << "\n";
    if ( abs(q) > EPSILON ) {
	ERR_RET("vertex check: star of vertex does not add up");
    }

}




COORD Vertex::total_angle()
{

  
   
    OEdgePtrIter i,j;
    COORD s=0;
    
    if( deleted())
	return(0);

    
    j = i = out_edges.begin();
    j++;

    while( j!= out_edges.end() ) {
	
	s = s+angle((*i)->vec_cx(), (*j)->vec_cx());
          
     i++;
     j++;

    }

    s = s+ angle(out_edges.back()->vec_cx(),out_edges.front()->vec_cx());

    return(s);
}


int Vertex::total_angle_over_pi()
{

    COORD a = total_angle();

#ifdef USE_QUAD
    int j = round_to_int(a/MY_PI);
#else
    int j = (int)rint(a/MY_PI);
#endif

    /*
    cout << " a= " << a;
    cout << " j= " << j << endl; 
    */

    if ( abs(j*MY_PI - a) > 10*EPSILON ) {
	std::cerr << abs(j*MY_PI - a) << endl; 
	ERR_RET("total_angle_over_pi: bad angle");
    }
    return(j);
}





