#include "libpolygon/two_complex.h"


/*   generic I 	  	       	 are_passing           *   starting
|\     	       	       	     |\                        |\
| \	  		     | \                       | \
|  \	  		     |  \                      |  \
|   \	  		     |   \                     |   \
|    \ 	       	       	     |    \                    |    \
|     \        	       	     |     \                   |     \
|     	\ 		     |      \                  |      \
|     	 ^  ne		     |       ^  ne             |       \
|     	  \    	       	     |        \                |        \
|     	   \		     |         \               |c_edge   \
|     	    \		     |          \              |          <
|     	     \		     |           \             |           \
|    cf	      \		     |     cf     \            |    cf      \
|     	       \	     |             \           |             \
|     	       	\	     |              \          |              \
|     	    	 \	     |               \         |               \
|     start    	  \    	     |                \        |   start        \
|      	   /   	   \   	     |                 \       |  /              \
|     	  /  	    \ 	     |                  \   /  | /                \
|        /     	     \ 	     |                   \ /   |/                  \
*------------->-------\	     *----------->--------+    +------------->-------
       	      c_edge  		      c_edge                here c_edge points
       	       	       	       	       	       	       	    down
      * is vert_pos();
 */

/*   generic II 	       	 still_passing           *   stop passing
|\     	       	       	     |\                        |\
| \	  		     | \                       | \
|  \	  		     |  \                      |  \
|   \	  		     |   \                     |   \
|    \ 	       	       	     |    \                    |    \
|     \        	       	     |     \                   |     \
|     	\ 		     |      \                  |      \
|     	 ^  ne		     |       ^  ne             |       \
|     	  \    	       	     |        \                |        \
|     	   \		     |         \               |c_edge   \
|     	    \		     |          \              |          <
|     	     \		     |           \             |           \
|    cf	      \		     |     cf     \            |    cf      \
|     	       \	     |             \           |             \
|     	       	\	     |              \          |              \
|     	    	 \	     |               \         |               \
|     start    	  \    	     |                \        |   start        \
|  \   	       	   \   	     |                 \       |  /              \
|   \  	       	    \  	     |                  \   /  | /                \
|    \         	     \ 	     |                   \ /   |/         ne       \
*------------->-------\	     *----------->--------+    +------------->-------
       	      c_edge  		      c_edge                here c_edge points
       	       	       	       	       	       	       	    down
      * is vert_pos();
 */


template<typename PointT>
class DMap       //developing map class
{

public:
    DMap(const Dir<PointT> dir);
    Dir<PointT>& _start();      // this is the Dir we are following

    OEdgeIter& _next_edge(); //next face will be on the edge on the other side of this
    void advance();  // go to the next face (following DMap._start() )
    FacePtr current_face();  //the face containing the current edge
    OEdgeIter current_edge(); // the current edge cut by the line we are following. Always oriented to that the head of the edge is CCW from the line.

    PointT& _current_vert_pos();  // the position of the vertex at the head of 
                       // current_edge, where base of start is (0,0)
                       // this is our candidate

    bool going_to_hit(); //are we about to hit a vertex along _start()?
    Dir<PointT>& _vertex_to_hit();// if yes, this is the vertex we will hit


    Dir<PointT> current_vert_Dir(); // the Dir whose vec is -vert_pos(), oriented from vert_pos towards the base of start;

    PointT& _get_cf_offset(); // will eventually remove this

private:
    void setup();
    Dir<PointT> strt;
    OEdgeIter c_edge;
    PointT cf_offset;   //offset of current face, i.e c_edge.in_face();

    bool m_going_to_hit;
    Dir<PointT> m_vertex_to_hit;
    
    OEdgeIter m_next_edge;

    PointT m_current_vert_pos;
    Dir<PointT> m_current_vert_Dir;

    PointT  m_next_candidate; // internal use by setup() only

//    bool are_passing; //are we currently passing on the left
};

template <typename PointT>
DMap<PointT>::DMap(const Dir<PointT> dir)
{

    strt = dir;
    c_edge = (*dir.ep)->prev_edge();
    cf_offset = -c_edge->_tail_offset<PointT>();
    setup();
}

template <typename PointT>
void DMap<PointT>::setup()    // compute cached variables
{

    // if( aligned(c_edge->vec<PointT>(), _start().vec )) {
    // 	ERR_RET("DMap setup: c_edge and start are aligned");
    // }
    // if ( !CCW(_start().vec, -c_edge->vec<PointT>() )) {
    // 	ERR_RET("DMap setup: start and -cedge not CCW");
    // }


    m_going_to_hit = false;
    OEdgeIter ne = c_edge->next_e;
    m_next_candidate = cf_offset;
    m_next_candidate += ne->_tail_offset<PointT>();

    if( aligned( m_next_candidate,_start().vec ) ) {
	m_going_to_hit = true;
	m_vertex_to_hit = Dir<PointT>(ne->pair_e);
	m_vertex_to_hit = m_vertex_to_hit.RotateCwToVec(-m_next_candidate);

	ne = ne->next_e;

    } else if ( CCW(m_next_candidate, _start().vec)){
	ne = ne->next_e;
    }
    m_next_edge = ne; 

    m_current_vert_pos = cf_offset;
    m_current_vert_pos += c_edge->_head_offset<PointT>();

}

template <typename PointT>
bool DMap<PointT>::going_to_hit()
{
    return(m_going_to_hit);
}

template <typename PointT>
Dir<PointT>& DMap<PointT>::_vertex_to_hit()
{
    return(m_vertex_to_hit);
}

template <typename PointT>
OEdgeIter& DMap<PointT>::_next_edge()
{
    return(m_next_edge);
}


template <typename PointT>
void DMap<PointT>::advance()
{

    cf_offset += _next_edge()->template _head_offset<PointT>(); 
    cf_offset -= _next_edge()->pair_e->template _tail_offset<PointT>();
    c_edge = _next_edge()->pair_e;
    setup();

}



template <typename PointT>
Dir<PointT> DMap<PointT>::current_vert_Dir()
{

    Dir<PointT> n(c_edge);
    n = n.RotateCwToVec(-_current_vert_pos());

    return(n);


}

template <typename PointT>
PointT& DMap<PointT>::_get_cf_offset()
{

    return(cf_offset);
}


template <typename PointT>
Dir<PointT>& DMap<PointT>::_start()
{
    return strt;
}

template <typename PointT>
OEdgeIter DMap<PointT>::current_edge()
{
    return c_edge;
}

template <typename PointT>
FacePtr DMap<PointT>::current_face()
{

    return c_edge->in_face();
}

template <typename PointT>
PointT& DMap<PointT>::_current_vert_pos()
{

//    return cf_offset + c_edge->_head_offset<PointT>();
    return m_current_vert_pos;
}



template <typename PointT>
VertexPtr TwoComplex::SweepNextLeft(const Dir<PointT>& strt, Dir<PointT>& end_Dir, COORD len2, COORD threshold) 
{

    DMap<PointT> D(strt);

    VertexPtr return_vertex = NULL;

    if ( D.going_to_hit() && norm(D._vertex_to_hit().vec) < len2 && 
		 D._vertex_to_hit().v->relevant() ) {
	return_vertex = D._vertex_to_hit().v;
    }

    int count = 0; 
    
    end_Dir = D.current_vert_Dir();
    PointT end_pos = D._current_vert_pos();

    if ( verbose >= 4 ) {
	cout << "SweepNextLeft: E" << D.current_edge()->id()<< " F"<< D.current_face()->id() 
	     << " next edge E" << D._next_edge()->id() <<" count = " << count << " cf_offset = " << D._get_cf_offset() << endl;
    }


    do {
	D.advance();
	count++;

	if ( verbose >= 4 ) {
	    std::cout << "SweepNextLeft: E" << D.current_edge()->id();
	    std::cout << " F"<< D.current_face()->id();
	    std::cout << " next edge E" << D._next_edge()->id();
	    std::cout <<" count = " << count; 
	    std::cout  << " cf_offset = " << D._get_cf_offset() << endl;
	}


	if ( D.going_to_hit() ) {
	    if ( verbose >= 3 ) {
		std::cout << "aligned: " << D._vertex_to_hit().v->id() << endl;
	    }
	    if ( norm(D._vertex_to_hit().vec) < len2 && 
		 D._vertex_to_hit().v->relevant() ){

		return_vertex = D._vertex_to_hit().v;
	    }
	}



	if( !aligned( D._current_vert_pos(),end_pos) &&
	    CCW(D._current_vert_pos(), end_pos) && 
	    norm(D._current_vert_pos()) < len2 ) {
	    if ( verbose >= 3 ) {
		std::cout << "New Candidate V"<<end_Dir.v->id();
		std::cout <<" " << end_pos <<endl;
	    }
	    end_pos = D._current_vert_pos();
	    end_Dir = D.current_vert_Dir();
	}
    } while( norm(D._get_cf_offset()) <= len2 + threshold ); 
	
    return(return_vertex);

}
template <typename PointT>
bool TwoComplex::NewFollowDir(const Dir<PointT>& strt, Dir<PointT>& end, COORD len2)
{

    DMap<PointT> D(strt);
    
    while( norm(D._get_cf_offset()) < len2 ) {
	if ( verbose >= 4 ) {
	    std::cout << "F"<<D.current_face()->id() <<  "..";
	}
	
	if ( D.going_to_hit() ) {
	    if ( verbose >= 3 ) {
		std::cout << "NewFollowDir: aligned: " << D._vertex_to_hit().v->id() << endl;
	    }
	    if ( norm(D._vertex_to_hit().vec) < len2 && 
		 D._vertex_to_hit().v->relevant() ){

		end = D._vertex_to_hit();
		return true;
	    }
	}
	D.advance();
    }
    if( verbose >=4 ) 
	std::cout << endl;

    return(false);
}



template <typename PointT>
void TwoComplex::FindCrossSaddle(const Dir<PointT>& strt, 
				 Dir<PointT>& cross_saddle)
{
    DMap<PointT> D(strt);
    int count = 0; 
    
    cross_saddle = D.current_vert_Dir();
    COORD smallest_distance2 = d_point_line2(D._current_vert_pos(), 
					     D._start().vec);

    if ( verbose >= 4 ) {
	std::cout << "CS: E" << D.current_edge()->id();
	std::cout << " F"<< D.current_face()->id();
	std::cout << " next edge E" << D._next_edge()->id();
	std::cout <<" count = " << count;
	std::cout << " cf_offset = " << D._get_cf_offset() << endl;
    }


    do {
	D.advance();
	count++;

	if ( verbose >= 4 ) {
	    std::cout << "CS: E" << D.current_edge()->id(); 
	    std::cout << " F"<< D.current_face()->id();
	    std::cout << " next edge E" << D._next_edge()->id();
	    std::cout <<" count = " << count;
	    std::cout << " cf_offset = " << D._get_cf_offset() << endl;
	}

	if((d_point_line2(D._current_vert_pos(),D._start().vec) 
	    < smallest_distance2 )
	   && CCW(D._start().vec, D._current_vert_pos() ) //redundant
	    && D.current_vert_Dir().v->relevant() ) {
	    
	    if( verbose>= 3 ) {
		std::cout << "CS new candidate " << D._current_vert_pos() << endl;
	    }
	    
	    smallest_distance2=d_point_line2(D._current_vert_pos(), 
					     D._start().vec);
	    cross_saddle = D.current_vert_Dir();
	    
	    cross_saddle.Check(); //redundant
	}

	
    } while( sqrt(norm(D._get_cf_offset())) 
	     <= sqrt(norm(D._start().vec))+10*sqrt(get_area()));

    return;

}


template <>
void TwoComplex::DrawSaddle<Point>(const Dir<Point> &strt, COORD len2,int id, COORD cyl_length)
{

    DMap<Point> D(strt);
    
    while( norm(D._get_cf_offset()) < len2 ) {
	if ( verbose >= 4 ) {
	    std::cout << "F"<<D.current_face()->id() <<  "..";
	}
	
	Segment s;

	Point p1(0,0);
	Point p2 = (follow_depth+100.0)*D._start().vec/abs(D._start().vec);

	Point q1 = D._get_cf_offset()+D.current_edge()->_head_offset<Point>();
	Point q2 = D._get_cf_offset()+D.current_edge()->_tail_offset<Point>();

//	Point q1 = cf_offset + c_edge->_head_offset;
//	Point q2 = cf_offset + c_edge->_tail_offset;


	if ( verbose >= 5 ) {
	    std::cout << "p1="<<p1<<" p2="<<p2 <<" q1="<<q1<<" q2="<<q2 <<endl;
	}

	

	if( aligned(q1, D._start().vec)) {
	    ERR_RET("DrawSaddle: q1 aligned");
	}
	if ( aligned(q2, D._start().vec) ) {
	    s.head = q2;
	} else {
	    if ( ! intersect_segment(p1,p2-p1,q1,q2-q1,s.head )) {
		ERR_RET("DrawSaddle: bad intersection");
	    }
	}	    


	
	Point r1 = D._get_cf_offset()+D._next_edge()->_head_offset<Point>();
	Point r2 = D._get_cf_offset()+D._next_edge()->_tail_offset<Point>();

 
	if ( verbose >= 5 ) {
	    std::cout << "r1="<<r1<<" r2="<<r2 <<endl;
	}



	bool r1_aligned = aligned(r1, D._start().vec);
	bool r2_aligned = aligned(r2, D._start().vec);

	if ( r1_aligned && r2_aligned ) {
	    s.head = r1;
	    s.tail = r2; 
	} else if ( r1_aligned ) {
	    s.tail = r1;
	} else if ( r2_aligned ) {
	    s.tail = r2;
	} else { // neither aligned
	    if ( ! intersect_segment(p1,p2-p1,r1,r2-r1,s.tail )) {
		ERR_RET("DrawSaddle: bad intersection");
	    }
	}

	if ( id < 0 ) {
	    s.cyl_on_left = -id;
	    s.cyl_on_left_length = cyl_length;
	    Point t = s.head; s.head = s.tail; s.tail = t;
	} else {
	    s.cyl_on_left = id;
	    s.cyl_on_left_length = cyl_length;
	}


	

	if ( abs(s.head - s.tail ) > EPSILON ) {
	    s.head = s.head - D._get_cf_offset();
	    s.tail = s.tail - D._get_cf_offset();
	    if ( ! billiard_mode ) {
		D.current_face()->AddSegmentToDraw(s);
	    } else { //billiard mode
		FacePtr f_orig;
		BigPointQ tmp, tmp2;
		tmp2.cx = s.head;
		f_orig = D.current_face()->transform_to_original(tmp2, tmp);
		s.head = tmp.cx;
		tmp2.cx = s.tail;
		D.current_face()->transform_to_original(tmp2, tmp);
		s.tail = tmp.cx;
		f_orig->AddSegmentToDraw(s);
	    }
		
	}
	

	if ( D.going_to_hit() && D._vertex_to_hit().v->relevant()) {
	    if ( verbose >= 3 ) {
		std::cout << "Draw_Saddle: aligned: " << 
		    D._vertex_to_hit().v->id() << endl;
	    }
	    return;
	}
	D.advance();
    }
    if( verbose >=4 ) 
	std::cout << endl;

    return;
}






Summary smry; /* global variable */


template <typename PointT> 
int TwoComplex::SweepNew(COORD depth, Dir<PointT> start_dir,
			 COORD GoalTotalAngle)
{

//    VertexPtr v0 = start_dir.v;
    Dir<PointT> old_dir = start_dir;
    int count = 0;
    COORD TotalAngle = 0;
    Summary *sm = &smry;

    SaddleConf sc;

    VertexPtr c;

    Dir<PointT> new_dir, tmp_dir;

    int count_same = 0;
    int group_count = 0;
    COORD group_angle = 0.0;

    PointT pending_vec[2];
    pending_vec[0] = pending_vec[1] = PointT(0,0);

    int n_pending = 0;
    int investigated_last = false;

    COORD longest = MaxEdge()->len();
    COORD threshold = 2*(2*depth*longest + longest*longest+1);

    while ( TotalAngle < GoalTotalAngle ) {

	c = SweepNextLeft<PointT>(old_dir, new_dir, depth*depth,threshold);

	if( verbose >= 2 ) {
	    std::cout << "c: " << c << "  " << old_dir.vec << " "<< -new_dir.vec;
	    if ( CCW( old_dir.vec, -new_dir.vec ) ) {
		std::cout << " +";
	    } else {
		std::cout << " -";
	    }
	    std::cout << angle(old_dir.vec, -new_dir.vec) << "\n";
	}

	if ( c != NULL && investigated_last == false
	     && norm(old_dir.vec_cx()) < depth*depth) {
	    if ( c == old_dir.v ) {
		count_same++;
	    }
	    pending_vec[n_pending++] = old_dir.vec;
//	    if ( TotalAngle == 0 ) { /* something hit on the very start */
//		ERR_RET("new_sweep: bad choice of start");
//	    }
	}

	COORD a = angle(old_dir.vec_cx(), -new_dir.vec_cx());
               /* assume this is smaller then pi*/

	TotalAngle += a;
	group_angle += a;

	if ( new_dir.v->relevant() && TotalAngle < GoalTotalAngle
	     && norm(new_dir.vec_cx()) < depth*depth ) {
	    if ( new_dir.v == old_dir.v ) {
		count_same++;
	    }
	    pending_vec[n_pending++] = -new_dir.vec;
	    investigated_last = true;
	} else {
	    investigated_last = false;
	}

	for ( int i = 0; i < n_pending; i++ ) {

	    too_close_flag = false; 

	    InvestigateVec(pending_vec[i], follow_depth*follow_depth,sc, *sm);

	    //debugging

/*
	    if( abs(sc.get_orig_min_saddle_length() - 21.6226 ) < 0.0001 ) {
		std::cout << "Found Bad Pattern\n";
		sc.print(std::cout);
		std::cout << endl;

		SaddleConf sc2;

		verbose = 5;


		InvestigateVec(pending_vec[i], follow_depth*follow_depth,
			       sc2, *sm);

		exit(0);

//		verbose = 0;

	    }
*/

	    if ( too_close_flag ) {

		smry.reject_count++;
		too_close_flag = false;

		continue;
	    }

	    
	    if ( sc.n_saddles() > 0 ) {
		/* add sc to the summary */
		int tag = smry.add_one_conf(sc); 
		
		//draw if needed
//		std::cout << "sc.get_orig_min_saddle_length = " << sc.get_orig_min_saddle_length() << " draw_saddle_length = " << draw_saddle_length << endl;
//		std::cout << "tag = " << tag 
//		    << " draw_tag = " << draw_tag << endl; 

		if ( abs(sc.get_orig_min_saddle_length() -
			 draw_saddle_length)/draw_saddle_length < 0.001  &&
		     ( draw_tag < 0 || tag == draw_tag )) {
		    std::cout << "Found it: n_saddles = " <<
			sc.n_saddles() << endl; 		
		    if( draw_saddles ) {
			std::cout << "Drawing Saddles" << endl;
			sc.DrawSaddles();
			S->make_pcomplexes();
			my_ostream saddle_stream("saddle");
			S->NewDraw(saddle_stream);
			saddle_stream.close();
		    }
		    if ( draw_cylinders ) {
			std::cout << "Drawing Cylinders" << endl;
			sc.DrawCylinders();
			S->make_pcomplexes();
			my_ostream saddle_stream("cylinders");
			S->NewDraw(saddle_stream);
			saddle_stream.close();
		    }
		    exit(0);
		}
	    
		
		count++;
		group_count++;
	    }
	    if( group_count == mc_group ) {

		if( ! quiet  && !individual ) {
		    /* issue running total report */
		    sm->print(std::cout, TotalAngle/(GoalTotalAngle*n_slices),
			      TotalAngle/GoalTotalAngle,
			      get_area(), depth);
		}
		/* clean up */
		group_count = 0;
		group_angle = 0;
		sm->clear_group();
	    }
	}
	n_pending = 0;

	old_dir = old_dir.RotateCCwToVec(-new_dir.vec);
    }
    return(count_same);
}



bool Vertex::relevant()
{

    if ( !euclidean || id() == start_vertex || id() == end_vertex ) {
	return(true);
    }
    return(false);
}

template <typename PointT>
void TwoComplex::InvestigateVec(PointT vec, COORD len2, SaddleConf& sc,
				Summary& smry)
{


    Dir<PointT> start, end, tmp;

    sc.clear();

    if ( verbose >= 1 ) {
	 std::cout << "InvestigateVec: " << vec << "\n";
    }

    if ( individual ) {
	Point vec_cx = to_cx(vec);
	COORD real = vec_cx.real();
	COORD imag = vec_cx.imag();
	//fprintf(out_f,"%.24Lf %.24Lf\n",real,imag);
	std::cout << " " << real  << " " <<  imag  << endl;
    }

    for(VertexPtrIter i = vertices.begin(); i!=vertices.end(); i++ ) {
	if ( (*i)->deleted() || (! (*i)->relevant()) ) {
	    continue;
	}
	start = Dir<PointT>((*i), vec);

	for(int j = 0; j < (*i)->int_angle/2 ; j++ ) {
	    if ( verbose >= 3 ) {
		fprintf(out_f, "Following Dir: j=%d start = V%d E%d\n",j,
			start.v->id(), (*start.ep)->id());
	    }
	    if ( verbose >= 5 ) {
		ClearSegmentsToDraw();
		dl.clear();
	    }

	    //big hack
	    //DELTA = 1.0E-4;
	    //EPSILON = 1.0E-5;

	    bool c = NewFollowDir(start, end, len2);
/*
	    if ( verbose >= 5 ) {
		char buf[1000];

		sprintf(buf,"saddle%d.tri",j);
		ofstream* saddle_stream = new ofstream(buf);
		make_pcomplexes();
		NewDraw(*saddle_stream);
		saddle_stream->close();
		delete saddle_stream;

	    }
*/
	    if ( c ) {
	        if ( verbose >= 2 ) {
		    fprintf(out_f,"start = V%d E%d end = V%d E%d..",start.v->id(),
			    (*start.ep)->id(), end.v->id(), (*end.ep)->id());
		}
		try {
		    if( verbose >= 2 ) {
			fprintf(out_f,"adding\n");
		    }

		    //hack for now
		    Dir<Point> tmp_start, tmp_end;
		    alg_tI tmp_algt;
		    tmp_start.vec = -(end.vec_cx());
		    tmp_start.v =   start.v;
		    tmp_start.ep  = start.ep;

		    tmp_end.vec = end.vec_cx();
		    tmp_end.v = end.v;
		    tmp_end.ep = end.ep;
		    if ( int_field_arithmetic ) {
			tmp_algt = -end.vec_algtI();
		    }

		    sc.add_saddle(tmp_start, tmp_end, tmp_algt);


		} catch (vert_bad_angle error ) {
		    if ( verbose >=2 ) {
			fprintf(out_f,"bad angle exception\n");
		    }
		    smry.bad_angle_count++;
		} catch (vert_index_taken error) {
		    if ( verbose >= 2 ) {
			fprintf(out_f,"..vert index taken\n");
		    }
		    smry.weird_count++;
		}
	    }
//	    start.RotateCCwToVec(-vec, tmp);
//	    tmp.RotateCCwToVec(vec, start);


	    tmp = start.RotateCCwToVec(-vec);
	    start = tmp.RotateCCwToVec(start.vec); 
	}
    }

/*
    if ( verbose >= 1 || individual ) {
	//debugging hack
	int tmp_verbose = verbose;
	verbose = 0;

	sc.print(std::cout);
	std::cout << "\n";

	//debugging hack
	verbose = tmp_verbose;

    }
*/

    if( show_lengths || show_cyls ) {
	sc.renorm_lengths();
    }

}

COORD TwoComplex::GetSaddles(Dir<Point>& start, COORD len2, int N)
{

    COORD TotalAngle = 0;
    int number = 0;
    COORD factor;

    Dir<Point> old_dir, new_dir, tmp_dir;

    int count_same = 0;



    old_dir = start;

    while( number < N ) {

	SweepNextLeft<Point>(old_dir, new_dir, len2,10.0); /* hack */
	if ( number == 0  ) {
	    factor = 2.0;
	} else {
	    factor = 1.0; /* FIX LATER */
	}
//	std::cout << "c: " << c << "  " << -new_dir.vec << " "
//	     << angle(old_dir.vec, -new_dir.vec ) << " ";

	TotalAngle += factor*angle(old_dir.vec, -new_dir.vec);
               /* assume this is smaller then pi*/

	if ( ! new_dir.v->euclidean ) {

//	    std::cout << "NOT EUCLIDEAN" <<"\n";
	    if ( new_dir.v == old_dir.v ) {
		count_same++;
		number++;  /* FIX */
	    }


	}
	else {
//	    std::cout << "EUCLIDEAN" << "\n";
	}



	tmp_dir = old_dir.RotateCCwToVec(-new_dir.vec);
	old_dir = tmp_dir;
    }

    return(TotalAngle*count_same/number);
}

COORD TwoComplex::RandomShoot(VertexPtr v0, COORD depth, int M)
{


    COORD TotalAngle = 0.0;
    Dir<Point> old_dir(v0, Point(1,0));
    Dir<Point> new_dir;
    int count = 0;
    COORD theta;
    COORD ta;


    while ( count < 1.0*M/mc_group ) {
	theta = v0->total_angle()*my_random()/RANDOM_MAX;
//	std::cout << "theta: " << theta << "\n";
	new_dir = old_dir.RotateF(theta);
	old_dir = new_dir;

	fprintf(out_f,"group %3d:  ", count+1);

	ta = GetSaddles(old_dir, depth*depth, mc_group );
	TotalAngle = TotalAngle + ta;


	COORD s1 = v0->total_angle()*mc_group/ta;
	std::cout << s1*get_area()*MY_PI/(6*depth*depth) << " ( " <<
	       s1*get_area()/(MY_PI*depth*depth);

	std::cout << ") raw = " << s1 << endl;


	count++;
    }
    return(TotalAngle);
}



template int TwoComplex::SweepNew<BigPointI>(COORD depth, 
					     Dir<BigPointI> start_dir, 
					     COORD GoalTotalAngle);

template int TwoComplex::SweepNew<Point>(COORD depth, 
					     Dir<Point> start_dir, 
					     COORD GoalTotalAngle);



template void TwoComplex::FindCrossSaddle<BigPointI>(const Dir<BigPointI>& strt,
					       Dir<BigPointI>& cross_saddle);


template void TwoComplex::FindCrossSaddle<Point>(const Dir<Point>& strt,
					       Dir<Point>& cross_saddle);


