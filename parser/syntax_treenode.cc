#include "../include/global.h"
#include "../include/syntax_treenode.h"

namespace dh{

TreeNode::TreeNode( const ::std::string& c, const int& level)
{
	this->_data = c;
	this->_sibling = nullptr;
	this->_level = level;
}

TreeNode::TreeNode( const int& val, const int& level)
{
	this->_data = val;
	this->_sibling = nullptr;
	this->_level = level;
}

TreeNode::TreeNode( const float& val, const int& level)
{
	this->_data = val;
	this->_sibling = nullptr;
	this->_level = level;
}

void TreeNode::appendChild( const NodePtr& ptr)
{
	this->_children.push_back(ptr);
}

void TreeNode::setSibling( const NodePtr& ptr)
{
	this->_sibling = ptr;
}

void TreeNode::setNodeKind( const int& _nk)
{
	this->_nodekind = _nk;
}

void TreeNode::setKind( const int& _k)
{
	this->_kind = _k;
}

void TreeNode::setType( const int& _t)
{
	this->_type = _t;
}

int TreeNode::getNodeKind() const
{
	return this->_nodekind;
}

int TreeNode::getKind() const
{
	return this->_kind;
}

int TreeNode::getType() const
{
	return this->_type;
}

int TreeNode::getChildSize() const
{
	return this->_children.size();
}

::std::vector< NodePtr > TreeNode::getChildren() const
{
	return this->_children;
}

DataType TreeNode::getData() const
{
	return this->_data;
}

NodePtr TreeNode::getSibling() const
{
	return this->_sibling;
}

void TreeNode::setStrVal( const ::std::string str)
{
	this->strval = str;
}

::std::string TreeNode::getStrVal()
{
	return this->strval;
}

int TreeNode::getLevel() const
{
	return this->_level;
}
}
