
Worker::GetAssignment
AssignmentReport Worker::ProcessAssignment(const Assignment &asn)
{
	slv.InitSolver(GetSWrapperDataFromAssignment(asn));
	slv.Process();
	return BBRep2AsnRep(slv.GetReport());
}

Worker::UploadResult

Worker::MainJobCycle()
{
	Assignment asn;
	AssignmentReport rep;

	asn = GetAssignment();
	rep = ProcessAssignment(ast);
	UploadAssignmentReport(rep);
}


