class RiseFallSearch
{

PointStats RiseFallSearch (
		Evaluator& eval,
		TabooSearch& searchEngine,
		const int num_points,
		const int groundLevel,
		const int stallLimit,
		PointId basePoint = PointId ())
{
	if (basePoint == PointId ())
		basePoint = PointId(eval.guessing_vars.size(), 0);


	assert (CountOnes(basePoint) <= groundLevel);
	LOG(INFO) << " STAGE 1 - RISE";
	for (int i = groundLevel; (searchEngine.origin_queue_.empty() && (i <= eval.guessing_vars.size())); ++i)
		for (auto r: eval (searchEngine.GenerateRandomPoints (i, 10 /* num points */, basePoint)))
			searchEngine.AddPointResults (r, 2 /* Sat threshold */);

	LOG(INFO) << " STAGE 2 - FALL";
	PointStats lastRecord;
	for (int stallCount = 0; (!searchEngine.origin_queue_.empty() && (stallCount < stallLimit)); ++stallCount)
		for (auto r: eval (searchEngine.GenerateNewPoints (num_points, basePoint)))
			if (searchEngine.AddPointResults (r)->id == eval.record.id)
			{
				stallCount = 0;
				lastRecord = eval.record;
			}

	while(!searchEngine.origin_queue_.empty ()) 
		searchEngine.origin_queue_.pop ();
	eval.ResetCurrentRecord ();

	return lastRecord;
}


RiseFallSearch::Search (
