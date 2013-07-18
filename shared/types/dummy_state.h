#ifndef DUMMY_STATE_H
#define DUMMY_STATE_H

struct dummy_state
{
  int automatonState;
  agent_state state;
  bool operator ==(const dummy_state & other) const
  {
    if ((*this).automatonState==other.automatonState)
      return true;
      else
	return false;
  }
  bool operator<(const dummy_state & other ) const
  {
    if ((*this).automatonState<other.automatonState)
      return true;
      else
	return false;
  }
};

#endif // DUMMY_STATE_H
