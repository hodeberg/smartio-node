/*
 * BufReadIf.h
 *
 *  Created on: 12 mar 2016
 *      Author: hodeberg
 */

#ifndef BUFREADIF_H_
#define BUFREADIF_H_


class BufReadIf {
public:
	virtual int get() = 0; // Returns -1 if empty
};


#endif /* BUFREADIF_H_ */
