/*
 * BufWriteIf.h
 *
 *  Created on: 10 mar 2016
 *      Author: hodeberg
 */

#ifndef BUFWRITEIF_H_
#define BUFWRITEIF_H_

class BufWriteIf {
public:
	virtual bool put(unsigned int c) = 0; // Returns false if full.
	virtual void reset();
};



#endif /* BUFWRITEIF_H_ */
