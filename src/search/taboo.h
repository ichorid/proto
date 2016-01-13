#ifndef SEARCH_TABOO_H_
#define SEARCH_TABOO_H_
#include <random>
#include <unordered_map>
#include <vector>
#include <queue>
#include "common.h"

// Comparator for point incapacity queue. Works via pointers.
class ComparePointIncapacity
{
	bool reverse;
public:
	
	ComparePointIncapacity(const bool revparam = false)
		: reverse(revparam)
	{}

	bool operator() (const PointStats* lhs, const PointStats* rhs) const
	{
		if (reverse) 
			return (lhs->best_incapacity < rhs->best_incapacity);
		else
			return (lhs->best_incapacity > rhs->best_incapacity);
	}
};
typedef std::priority_queue <PointStats*, std::vector <PointStats*>, ComparePointIncapacity> BestIncapacityQueue;
typedef std::unordered_map <PointId, PointStats*> PointStatsDB;

// fixme: зачем перегоняем строки в строки???
inline std::string Point2Bitstring(const PointId& p) { std::string out; for (int i=0; i<p.size(); ++i) out+= (p[i]==0 ? "0" : "1") ; return out;}
inline std::string Point2Varstring(const PointId& p) { std::string out; for (int i=0; i<p.size(); ++i) out+= (p[i]==0 ? "" : " "+std::to_string(i+1)); return out;}

class TabooSearch
{
public:
	TabooSearch();
	~TabooSearch();
	PointId ProcessPointResults (const PointId& point, const Results& results);
	
	/**
	* \brief Сохраняет результаты прогнозирования для указанной точки.
	* \param point [in] Точка, прогноз для которой сохраняем.
	* \param results [in] Результаты решения SAT-задач выборки в этой точке.
	*/
	void AddPointResults (const PointId& point, const Results& results);
	
	/**
	* \brief Возвращает информацию о последнем рекорде.
	* \fixme: название функции не отражает ее назначение.
	*/
	PointStats GetStats();

	// Очередь точек для проверки их окрестности, сортировка по лучшему прогнозу в данной точке
	BestIncapacityQueue origin_queue_;

	// Минимально допустимое число решеных SAT-задач в выборке
	int sat_threshold_ = 1;

private:

	// Хеш-таблица, содержащая все проверенные точки и информацию по ним
	PointStatsDB checked_points_;

	// Точка с лучшим последним рекордом
	PointStats* global_record_ = NULL;

	std::mt19937 rng;

	PointId GenerateNewPoint();
	//void LoadNewSample (int sample_size);

	// Service methods
	std::vector<PointId> GetUncheckedHammingNbhd (const PointId& point);
	inline bool PointChecked(const PointId& id) const { return checked_points_.count(id) > 0; }
};


#endif
