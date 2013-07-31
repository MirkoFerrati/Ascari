#include "nostop_discretized_area.h"

#include "nostop_ISLAlgorithm_packet.h"
#include "nostop_agent_position.h"
#include "nostop_line2D.h"

#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */

using namespace std;
using namespace NoStop;

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
////////////////		DISCRETIZED AREA		//////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
void DiscretizedArea::mergeReceivedData(ISLAlgorithm_packet* _data)
{
	for(size_t i = 0; i < m_lattice.size(); ++i)
	{
		if(_data->m_area[i] > 0)
			m_lattice[i]->increaseCounter();
	}
}

//////////////////////////////////////////////////////////////////////////
void DiscretizedArea::resetCounter()
{
	for(size_t i = 0; i < m_lattice.size(); ++i)
		m_lattice[i]->resetCounter();
}

//////////////////////////////////////////////////////////////////////////
SquarePtr DiscretizedArea::getSquare(AreaCoordinate _coord) const
{
	return getSquare(_coord.row, _coord.col);
}

//////////////////////////////////////////////////////////////////////////
SquarePtr DiscretizedArea::getSquare(int row, int col) const
{
	if( row >= m_numRow || row < 0 ||
		col >= m_numCol || col < 0 )
		return nullptr;

	return m_lattice.at(row * m_numCol + col);
}

//////////////////////////////////////////////////////////////////////////
AgentPosition DiscretizedArea::getRandomPosition() const
{
	// TODO
	AgentPosition pos;
	return pos;
}

//////////////////////////////////////////////////////////////////////////
bool DiscretizedArea::isOut(AgentPosition const& _pos) const
{
	Real2D l_point = _pos.getPoint2D();
	AreaCoordinate l_coord = this->getCoordinate( l_point );
	SquarePtr l_square = this->getSquare(l_coord);
	if(!l_square)
		return false;
	return l_square->isValid();
}

//////////////////////////////////////////////////////////////////////////
std::vector<SquarePtr> DiscretizedArea::getSquares() const
{
	return m_lattice;
}

//////////////////////////////////////////////////////////////////////////
bool isInside( 
	std::vector<Real2D> const& _external, 
	std::list< std::vector<Real2D> > const& _obstacles, 
	Box const& _box)
{
	bool l_inside = false;
	for(int i = 0; i < 4; ++i)
	{
		l_inside = Math::polygonContains(_external, _box.corner(i));
		if(l_inside)
		{
			bool l_outFromObstacle = true;
			for(std::list< std::vector<Real2D> >::const_iterator it = _obstacles.begin(); it != _obstacles.end(); ++it)
			{
				l_outFromObstacle = !Math::polygonContains(*it, _box.corner(i));
				if(!l_outFromObstacle)
				{
					l_inside = false;
					break;
				}
			}
			if(l_inside)
				return true;
		}
	}

	l_inside = Math::polygonContains(_external, _box.center());
	if(l_inside)
	{
		bool l_outFromObstacle = true;
		for(std::list< std::vector<Real2D> >::const_iterator it = _obstacles.begin(); it != _obstacles.end(); ++it)
		{
			l_outFromObstacle = !Math::polygonContains(*it,_box.center());
			if(!l_outFromObstacle)
			{
				l_inside = false;
				break;
			}
		}
		if(l_inside)
			return true;
	}
	return false;
}


//////////////////////////////////////////////////////////////////////////
DiscretizedArea::DiscretizedArea(
	std::vector<Real2D> const& _external, 
	std::list< std::vector<Real2D> > const& _obstacles, 
	int _numCol, 
	int _numRow)
{
	Box l_box;
	for(size_t i = 0 ; i < _external.size(); ++i)
		l_box.extend(_external[i]); 

	Real2D l_bottomLeft = l_box.corner(0);
	Real2D	l_bottomRight = l_box.corner(1);
	Real2D	l_topLeft = l_box.corner(2);
	Real2D	l_topRight = l_box.corner(3);

	double l_xdist = (l_bottomLeft- l_bottomRight).mod();
	double l_ydist = (l_bottomLeft-l_topLeft).mod();

	m_xStep = l_xdist / double(_numCol);
	m_yStep = l_ydist / double(_numRow);

	double l_xpos = 0.;
	double l_ypos = 0.;
	m_numRow = 0;
	m_numCol = 0;
	bool l_firstrow = true;
	while(l_ypos < l_ydist + Math::TOLERANCE)
	{
		double l_yorigin = l_ypos + l_bottomLeft[1];
		int l_count = 0;
		l_xpos=0.;
		while( (l_firstrow && l_xpos < l_xdist + Math::TOLERANCE) || (!l_firstrow && l_count < m_numCol))
		{
			double l_xorigin = l_xpos + l_bottomLeft[0];

			Real2D l_mincord(l_xorigin, l_yorigin);
			Real2D l_maxcord(l_xorigin+m_xStep, l_yorigin+m_yStep);

			Box l_boxSquare (l_mincord, l_maxcord);

			SquarePtr l_square = std::make_shared<Square>();
			l_square->setBoundingBox(l_boxSquare);

			if( isInside(_external, _obstacles, l_boxSquare) )
				l_square->setValid(true);
			else
				l_square->setValid(false);

			m_lattice.push_back(l_square);

			l_xpos+=m_xStep;
			if(l_firstrow)
				m_numCol++;
			l_count++;
		}
		l_ypos+=m_yStep;
		m_numRow++;
		l_firstrow = false;
	}
}

//////////////////////////////////////////////////////////////////////////
Real2D DiscretizedArea::getOrigin() const
{
	return m_lattice.at(0)->getBoundingBox().minCoord;
}

//////////////////////////////////////////////////////////////////////////
AreaCoordinate DiscretizedArea::getCoordinate(Real2D const& point) const
{
	const Real2D l_bottomLeft = m_lattice[0]->getBoundingBox().minCoord;
	const Real2D l_bottomRight = m_lattice[0]->getBoundingBox().corner(1);
	const Real2D l_topLeft = m_lattice[0]->getBoundingBox().corner(2);

	const Line2D l_xDir( l_bottomLeft, l_bottomRight );
	const Line2D l_yDir( l_bottomLeft, l_topLeft );

	Real2D l_prjVertical = l_yDir.projectPoint( point );
	Real2D l_prjOrizontal = l_xDir.projectPoint( point );

	double l_ydist = l_yDir.parameterAt(l_prjVertical);
	double l_xdist = l_xDir.parameterAt(l_prjOrizontal);

	AreaCoordinate l_coord;
	l_coord.col = int( floor(l_xdist / m_xStep) );
	l_coord.row = int( floor(l_ydist / m_yStep) );
	return l_coord;
}

//////////////////////////////////////////////////////////////////////////
SquarePtr DiscretizedArea::getSquare(Real2D const& V) const
{
	AreaCoordinate l_coord = getCoordinate(V);
	return this->getSquare(l_coord);
}

Real2D DiscretizedArea::getPosition(AreaCoordinate const& _coord) const
{
	SquarePtr l_square = this->getSquare(_coord);
	if(!l_square)
		return Real2D();
	return l_square->getBoundingBox().center();
}

bool g_reverse = false;
//////////////////////////////////////////////////////////////////////////
void DiscretizedArea::setRandomSquareValue()
{
	g_reverse = !g_reverse;
	/// Compute Random Position:
	srand ( (unsigned int) time(NULL) );

	int flag = m_lattice.size() / 100;

	for(size_t i = 0; i < m_lattice.size(); ++i)
	{
#if 1
		int l_valueSecret = rand() % 100;
		m_lattice[i]->setValue(l_valueSecret);
#else
		m_lattice[i]->setValue(g_reverse ? 100- i / flag:  i / flag);
#endif
	}
}

//////////////////////////////////////////////////////////////////////////
std::vector<AreaCoordinate> DiscretizedArea::getStandardApproachableValidSquares(AreaCoordinate const& _current) const
{
	std::vector<AreaCoordinate> result;
	if(_current.col != 0)
	{
		AreaCoordinate pos;
		pos.col = _current.col-1;
		pos.row = _current.row;
		if(this->getSquare(pos)->isValid())
			result.push_back(pos);
	}
	if(_current.col != m_numCol)
	{
		AreaCoordinate pos;
		pos.col = _current.col+1;
		pos.row = _current.row;
		if(this->getSquare(pos)->isValid())
			result.push_back(pos);
	}
	if(_current.row != 0)
	{
		AreaCoordinate pos;
		pos.col = _current.col;
		pos.row = _current.row-1;
		if(this->getSquare(pos)->isValid())
			result.push_back(pos);
	}
	if(_current.row != m_numRow)
	{
		AreaCoordinate pos;
		pos.col = _current.col;
		pos.row = _current.row+1;
		if(this->getSquare(pos)->isValid())
			result.push_back(pos);
	}
	return result;
}

//////////////////////////////////////////////////////////////////////////
void DiscretizedArea::addSpecialApproachableValidSquares(AreaCoordinate const& _current, std::vector<AreaCoordinate> & _loci) const
{
	if( _current.col != 0 )
	{
		if(_current.row != 0)
		{
			AreaCoordinate pos;
			pos.col = _current.col-1;
			pos.row = _current.row-1;
			if(this->getSquare(pos)->isValid())
				_loci.push_back(pos);
		}

		if(_current.row != m_numRow)
		{
			AreaCoordinate pos;
			pos.col = _current.col-1;
			pos.row = _current.row+1;
			if(this->getSquare(pos)->isValid())
				_loci.push_back(pos);
		}
	}

	if(_current.col != m_numCol)
	{
		if(_current.row != 0)
		{
			AreaCoordinate pos;
			pos.col = _current.col+1;
			pos.row = _current.row-1;
			if(this->getSquare(pos)->isValid())
				_loci.push_back(pos);
		}

		if(_current.row != m_numRow)
		{
			AreaCoordinate pos;
			pos.col = _current.col+1;
			pos.row = _current.row+1;
			if(this->getSquare(pos)->isValid())
				_loci.push_back(pos);
		}
	}
}

//////////////////////////////////////////////////////////////////////////
std::set< std::shared_ptr<Square> > DiscretizedArea::getVisibleSquares(AgentPosition const& _pos) const
{
	AreaCoordinate l_currentPos = this->getCoordinate(_pos.getPoint2D());

	std::set< std::shared_ptr<Square> > result;
	for( int row = 0; row < m_numRow; ++row )
	{
		for( int col = 0; col < m_numCol; ++col )
		{
			if( _pos.visible(m_lattice[row * m_numCol + col]) )
				result.insert(m_lattice[row * m_numCol + col]);
		}
	}
	return result;
}

//////////////////////////////////////////////////////////////////////////
void DiscretizedArea::resetValue()
{
	for(size_t i = 0; i < m_lattice.size(); ++i)
		m_lattice[i]->resetValue();
}

//////////////////////////////////////////////////////////////////////////
void DiscretizedArea::setThiefPosition(AgentPosition const& _pos)
{
	SquarePtr l_square = this->getSquare( _pos.getPoint2D() );
	if(l_square)
		l_square->setValue(100.);
	else
		assert(1 == 0);

	AreaCoordinate l_coord = this->getCoordinate( _pos.getPoint2D() );

	for(int i = -4; i < 5; ++i)
	{
		int row = l_coord.row + i;
		if(row < 0 || row >= m_numRow)
			continue;
		for(int j = -4; j < 5; ++j)
		{
			int col = l_coord.col + j;
			if(col < 0 || col >= m_numCol)
				continue;

			if(i == 0 && j == 0)
				continue; 

			double l_value = 100./ double( abs(i)+abs(j) );
			m_lattice[row * m_numCol + col]->setValue(l_value);
		}
	}
}

//////////////////////////////////////////////////////////////////////////
int DiscretizedArea::numberOfSquaresCoveredByGuards() const
{
	int l_total = 0;
	for(size_t i = 0; i < m_lattice.size(); ++i)
		if( m_lattice[i]->getTheNumberOfAgent() > 0)
			++l_total;

	return l_total;
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
///////////////////////		SQUARE		//////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

//bool Square::equals(std::shared_ptr<Square> _other) const
//{
//	return this->getBoundingBox().center().equals(_other->getBoundingBox().center());
//}

//////////////////////////////////////////////////////////////////////////
Real2D Square::vertex(int i) const
{
	int index = i;
	if(i == 2)
		index = 3;
	else if(i == 3)
		index = 2;
	return m_box.corner( index );
}

//////////////////////////////////////////////////////////////////////////
Real2D Square::agentVertex(int i) const
{
	int index = i;
	if(i == 2)
		index = 3;
	else if(i == 3)
		index = 2;
	Real2D l_center = m_box.center();
	Line2D l_line(m_box.corner( index ), l_center);
	double l_dist = (m_box.corner( index ) - l_center).mod();
	return l_line.pointFromOrigin(l_dist/2.);
}

//////////////////////////////////////////////////////////////////////////
void Square::setBoundingBox(Box const& _box)
{
	m_box = _box;
}

//////////////////////////////////////////////////////////////////////////
bool Square::isChanged() const
{
	return fabs(m_value - m_old_value) > Math::TOLERANCE;
}

//////////////////////////////////////////////////////////////////////////
void Square::resetValue()
{
	m_value = 0.;
}

//////////////////////////////////////////////////////////////////////////
void Square::setValid(bool valid) 
{
	m_valid = valid;
}

//////////////////////////////////////////////////////////////////////////
void Square::setValue(double _value) 
{
	m_old_value = m_value;
	m_value = _value;
}

