/*
 * tree.h
 *
 *  Created on: Sep 13, 2009
 *      Author: alainrc2005
 */

#ifndef TREE_H_
#define TREE_H_

#include "wsqInternal.h"

/* tree.c */
void build_wsq_trees(W_TREE w_tree[], const int,
                 Q_TREE q_tree[], const int, const int, const int);
void build_w_tree(W_TREE w_tree[], const int, const int);
void w_tree4(W_TREE w_tree[], const int, const int,
                 const int, const int, const int, const int, const int);
void build_q_tree(W_TREE w_tree[], Q_TREE q_tree[]);
void q_tree16(Q_TREE q_tree[], const int, const int, const int,
                 const int, const int, const int, const int);
void q_tree4(Q_TREE q_tree[], const int, const int, const int,
                 const int, const int);

#endif /* TREE_H_ */
