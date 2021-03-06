/*
 * Author: Dh
 * Update date: 2018.05.22
 * update info:
 * init syntax tree's node
 */

#ifndef _SYNTAX_TREE_H_
#define _SYNTAX_TREE_H_
#include <string>
#include <memory>
#include <boost/variant.hpp>
#include "global.h"

namespace dh{

class TreeNode;


typedef ::std::shared_ptr<TreeNode> NodePtr;
typedef ::boost::variant< ::std::string, int, float> DataType;

class TreeNode{
	public:
		TreeNode(const ::std::string& c, int lineno);
		TreeNode(const int& val, int lineno);
		TreeNode(const float& val, int lineno);

		// from left to right append child tree
		void appendChild( const NodePtr& ptr);

		// set sibling
		void setSibling( const NodePtr& ptr);

		// set some extra info
		void setNodeKind( const int& _nk);
		void setKind( const int& _k);
		void setType( const int& _t);
		void setData( const int& _v);
		void setData( const float& _v);
		void setData( const ::std::string& _v);

		// get some extra info
		int getNodeKind() const;
		int getKind() const;
		int getType() const;
		int getChildSize() const;
		int getLineno() const;

		// get children
		::std::vector< NodePtr > getChildren() const;

		// Type need user check
		DataType getData() const;

		// get next sibling
		NodePtr getSibling() const;

		void setStrVal( const ::std::string str);

		::std::string getStrVal();

	private:
		::std::vector< ::std::shared_ptr<TreeNode> > _children;
		NodePtr _sibling;
		::std::string strval;

		int _nodekind;
		int _kind;
		int _lineno;

		DataType _data;
		int _type;


};


}

#endif
