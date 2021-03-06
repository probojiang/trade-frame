/************************************************************************
 * Copyright(c) 2012, One Unified. All rights reserved.                 *
 * email: info@oneunified.net                                           *
 *                                                                      *
 * This file is provided as is WITHOUT ANY WARRANTY                     *
 *  without even the implied warranty of                                *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.                *
 *                                                                      *
 * This software may not be used nor distributed without proper license *
 * agreement.                                                           *
 *                                                                      *
 * See the file LICENSE.txt for redistribution information.             *
 ************************************************************************/

#pragma once

#include <cassert>
#include <sstream>
#include <vector>
#include <stdexcept>

#include <boost/shared_ptr.hpp>

namespace ou { // One Unified
namespace gp { // genetic programming

namespace NodeType {
  enum E { Bool = 0, Double, Count };  // used for indexing to correct lookup vector
}

namespace ParentLink {
  enum E { None = 0, Left, Center, Right };
}

namespace FactoryType {
  enum E { All = 0, Terminals, Nodes, Count };
}

class Node {
public:

  typedef boost::shared_ptr<Node> pNode_t;

  Node( NodeType::E ReturnType, NodeType::E ChildType );
  Node( const Node& rhs );
  virtual ~Node(void);

  const Node& operator=( const Node& rhs );

  bool IsTerminal( void ) const { return 0 == m_cntNodes; };
  unsigned int NodeCount( void ) const { return m_cntNodes; };
  NodeType::E ReturnType( void ) const { return m_ReturnType; };
  NodeType::E ChildType( void ) const  { return m_ChildType; };

  bool IsTimeSeries( void ) const { return m_bIsTimeSeries; };

  ParentLink::E ParentSide( void ) const { return m_eParentSide; };

  void TreeToString( std::stringstream& ) const;
  virtual void ToString( std::stringstream& ) const {};

  virtual bool EvaluateBoolean( void ) { throw std::logic_error( "EvaluateBoolean no override" ); };
  virtual double EvaluateDouble( void ) { throw std::logic_error( "EvaluateDouble no override" ); };

  Node& Parent( void ) { assert( 0 != m_pParent ); return *m_pParent; };

  // maybe use union here or change names to suit
  void AddLeft( Node* node );  // used with two node
  void AddCenter( Node* node );  // used with terminal node
  void AddRight( Node* node );  // used with two node

  Node& ChildLeft( void ) { assert( 0 != m_pChildLeft ); return *m_pChildLeft; };
  Node& ChildCenter( void ) { assert( 0 != m_pChildCenter ); return *m_pChildCenter; };
  Node& ChildRight( void ) { assert( 0 != m_pChildRight ); return *m_pChildRight; };

  Node* Replicate( void );  // makes a copy of this node, plus recursed copies of children

  virtual void PreProcess( void ) {}; // used with Genetic Programming Module for initializating time series

protected:

  NodeType::E m_ReturnType;
  NodeType::E m_ChildType;

  Node* m_pParent;

  Node* m_pChildLeft;
  Node* m_pChildCenter;
  Node* m_pChildRight;

  unsigned int m_cntNodes; // how many child nodes permitted by default (0 for terminal nodes, 1 for single, 2 for two nodes)
  bool m_bIsTimeSeries;

  ParentLink::E m_eParentSide;

  virtual Node* CloneBasics( void ) = 0;

private:
};

std::stringstream& operator<<( std::stringstream& ss, const Node& node );

template<typename N> // CRTP 
class NodeProxy: public Node {
public:
  NodeProxy( NodeType::E ReturnType, NodeType::E ChildType ): Node( ReturnType, ChildType ) {};
  NodeProxy( const NodeProxy& rhs ): Node( rhs ) {};
  const NodeProxy& operator=( const NodeProxy& rhs ) {
    if ( &rhs != this ) {
      Node::operator=( rhs );
    }
    return *this;
  }
  virtual ~NodeProxy( void ) {};

  virtual Node* CloneBasics( void ) {
    N* t = new N( dynamic_cast<N&>( *this ) );
    return t;
  }
protected:
private:
};

template<class N>
class NodeDouble: public NodeProxy<N> {
public:
  NodeDouble( void ): NodeProxy<N>( NodeType::Double, NodeType::Double ) {};
  ~NodeDouble( void ) {};
protected:
private:
};

template<class N>
class NodeBoolean: public NodeProxy<N> {
public:
  NodeBoolean( void ): NodeProxy<N>( NodeType::Bool, NodeType::Bool ) {};
  ~NodeBoolean( void ) {};
protected:
private:
};

} // namespace gp
} // namespace ou
