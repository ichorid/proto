#ifndef PROTO_WORKER_H_
#define PROTO_WORKER_H_
#include "common.h"

class Worker
{
public:
	void MainJobCycle();
	int master_id_ = 0;
	int data_tag_ = 0;
	int scans_limit_= 100000;
	Cnf cnf_;
protected:
	void UploadAssignmentReport(const SolverReport rep);
	Assignment WaitRecieveAssignment ();
	SolverReport ProcessAssignment(const Assignment &asn);
};

#endif
