#include <vector>
#include <string>

using namespace std;
typedef pair<int, string> identity;
const int typeCC = 3;
const int amountClassesCC = 39;
const int dimensionCC = 120;
const int solvedCC = 1503;
const vector<string> classNamesCC = { 
	"521drat",
	"521-noRevMin",
	"521-rnd",
	"521-sat",
	"AES",
	"AutoRemN2",
	"AutoRemN4",
	"AutoRemP2",
	"AutoRemP4",
	"CBMCdrat",
	"CommunityDRAT",
	"Cops",
	"DelayPP",
	"DelayPPNoL",
	"LABSDRAT",
	"LatePP",
	"Minisat",
	"N-N-N",
	"Riss3g",
	"Riss3gDRAT",
	"RissPlain",
	"Rissg3:plain_LIGHT",
	"SATdrat",
	"SGI-unsat",
	"STPdrat",
	"anbulagan-hardunsat",
	"bioinfo",
	"bits-dimacs",
	"community-generated",
	"gl23",
	"hardSolve600",
	"hgen",
	"ibm",
	"infer",
	"li_ye--md5",
	"plain_BVE:emaRestarts",
	"planning",
	"synthesis-AES",
	"unsolved"
 };
const vector<identity> featureIdentsCC = {
	{39, "clause-variable_polarity_entropy"},
	{38, "clause-variable_polarity_stdev"},
	{37, "clause-variable_polarity_mean"},
	{183, "RWH-2_entropy"},
	{81, "Exactly1Lit_steps"},
	{125, "Blocked_AND_gate_steps"},
	{103, "Full_AND_gate_steps"},
	{184, "RWH-2_valuesRate"},
	{12, "horn_clauses"},
	{170, "RWH-1_stdev"},
	{164, "Symmetry_computation_steps"},
	{162, "Symmetry2_steps"},
	{181, "RWH-2_mean"},
	{150, "Symmetry1_steps"},
	{196, "RWH-3_valuesRate"},
	{40, "clause-variable_polarity_valuesRate"},
	{144, "SymmTime1_stdev"},
	{28, "variable-clause_degree_stdev"},
	{143, "SymmTime1_mean"},
	{156, "SymmTime2_stdev"},
	{155, "SymmTime2_mean"},
	{138, "Symmetry0_steps"},
	{195, "RWH-3_entropy"},
	{132, "SymmTime0_stdev"},
	{188, "RWH-2steps"},
	{82, "Full_AND_gate_degree_zcount"},
	{182, "RWH-2_stdev"},
	{186, "RWH-2_Q2"},
	{176, "RWH-1steps"},
	{200, "RWH-3steps"},
	{187, "RWH-2_Q3"},
	{169, "RWH-1_mean"},
	{56, "Clause-Var_steps"},
	{171, "RWH-1_entropy"},
	{68, "Bin_Implication_graph_steps"},
	{29, "variable-clause_degree_entropy"},
	{194, "RWH-3_stdev"},
	{172, "RWH-1_valuesRate"},
	{17, "clause-variable_degree_stdev"},
	{18, "clause-variable_degree_entropy"},
	{158, "SymmTime2_valuesRate"},
	{1, "clauses"},
	{16, "clause-variable_degree_mean"},
	{48, "variable-clause_polarity_mean"},
	{19, "clause-variable_degree_valuesRate"},
	{49, "variable-clause_polarity_stdev"},
	{145, "SymmTime1_entropy"},
	{157, "SymmTime2_entropy"},
	{27, "variable-clause_degree_mean"},
	{193, "RWH-3_mean"},
	{87, "Full_AND_gate_degree_stdev"},
	{185, "RWH-2_Q1"},
	{131, "SymmTime0_mean"},
	{88, "Full_AND_gate_degree_entropy"},
	{50, "variable-clause_polarity_entropy"},
	{62, "bin_implication_graph_degree_stdev"},
	{179, "RWH-2_max"},
	{191, "RWH-3_max"},
	{146, "SymmTime1_valuesRate"},
	{30, "variable-clause_degree_valuesRate"},
	{51, "variable-clause_polarity_valuesRate"},
	{99, "Full_AND_gate_weights_valuesRate"},
	{98, "Full_AND_gate_weights_entropy"},
	{89, "Full_AND_gate_degree_valuesRate"},
	{57, "bin_implication_graph_degree_zcount"},
	{63, "bin_implication_graph_degree_entropy"},
	{4, "clauses_size_2"},
	{61, "bin_implication_graph_degree_mean"},
	{64, "bin_implication_graph_degree_valuesRate"},
	{97, "Full_AND_gate_weights_stdev"},
	{70, "Exactly1Lit_zcount"},
	{86, "Full_AND_gate_degree_mean"},
	{2, "vars"},
	{104, "Blocked_AND_gate_degree_zcount"},
	{199, "RWH-3_Q3"},
	{133, "SymmTime0_entropy"},
	{129, "SymmTime0_max"},
	{5, "clauses_size_3"},
	{198, "RWH-3_Q2"},
	{178, "RWH-2_min"},
	{119, "Blocked_AND_gate_weights_stdev"},
	{120, "Blocked_AND_gate_weights_entropy"},
	{134, "SymmTime0_valuesRate"},
	{180, "RWH-2_mode"},
	{197, "RWH-3_Q1"},
	{10, "clauses_size_8"},
	{192, "RWH-3_mode"},
	{141, "SymmTime1_max"},
	{7, "clauses_size_5"},
	{96, "Full_AND_gate_weights_mean"},
	{6, "clauses_size_4"},
	{109, "Blocked_AND_gate_degree_stdev"},
	{9, "clauses_size_7"},
	{190, "RWH-3_min"},
	{167, "RWH-1_max"},
	{110, "Blocked_AND_gate_degree_entropy"},
	{11, "clauses_size_>=_9"},
	{8, "clauses_size_6"},
	{75, "Exactly1Lit_stdev"},
	{153, "SymmTime2_max"},
	{76, "Exactly1Lit_entropy"},
	{45, "variable-clause_polarity_min"},
	{25, "variable-clause_degree_max"},
	{121, "Blocked_AND_gate_weights_valuesRate"},
	{111, "Blocked_AND_gate_degree_valuesRate"},
	{77, "Exactly1Lit_valuesRate"},
	{3, "clauses_size_1"},
	{189, "RWH-3_zcount"},
	{165, "RWH-1_zcount"},
	{108, "Blocked_AND_gate_degree_mean"},
	{118, "Blocked_AND_gate_weights_mean"},
	{74, "Exactly1Lit_mean"},
	{46, "variable-clause_polarity_max"},
	{59, "bin_implication_graph_degree_max"},
	{175, "RWH-1_Q3"},
	{140, "SymmTime1_min"},
	{152, "SymmTime2_min"},
	{84, "Full_AND_gate_degree_max"},
	{54, "variable-clause_polarity_Q3"},
	{23, "variable-clause_degree_zcount"}
};
const vector<int> allClassCC = { 
	0,
	23,
	15,
	37,
	33,
	23,
	2,
	13,
	27,
	24,
	37,
	27,
	12,
	18,
	16,
	35,
	22,
	1,
	1,
	15,
	3,
	36,
	26,
	14,
	21,
	14,
	21,
	26,
	14,
	14,
	14,
	9,
	28,
	22,
	18,
	22,
	2,
	26,
	28,
	33,
	1,
	25,
	30,
	26,
	29,
	1,
	18,
	10,
	21,
	29,
	10,
	35,
	35,
	0,
	35,
	37,
	2,
	33,
	2,
	35,
	2,
	2,
	2,
	35,
	10,
	10,
	26,
	35,
	2,
	35,
	35,
	9,
	1,
	34,
	9,
	29,
	21,
	1,
	0,
	10,
	2,
	15,
	0,
	12,
	1,
	1,
	1,
	32,
	2,
	33,
	12,
	0,
	18,
	2,
	1,
	18,
	31,
	12,
	18,
	3,
	26,
	15,
	16,
	33,
	33,
	35,
	3,
	3,
	3,
	14,
	3,
	3,
	3,
	14,
	3,
	14,
	3,
	14,
	3,
	3,
	3,
	14,
	14,
	3,
	3,
	1,
	14,
	2,
	2,
	2,
	2,
	3,
	3,
	2,
	3,
	3,
	3,
	3,
	3,
	3,
	3,
	2,
	3,
	2,
	1,
	36,
	29,
	35,
	11,
	22,
	26,
	30,
	9,
	9,
	3,
	24,
	0,
	0,
	11,
	0,
	1,
	3,
	3,
	0,
	16,
	30,
	7,
	29,
	35,
	5,
	10,
	14,
	13,
	33,
	18,
	1,
	21,
	14,
	10,
	6,
	21,
	14,
	22,
	11,
	2,
	13,
	1,
	2,
	14,
	10,
	2,
	0,
	10,
	12,
	13,
	32,
	32,
	32,
	9,
	14,
	5,
	14,
	22,
	10,
	10,
	22,
	12,
	31,
	21,
	21,
	15,
	10,
	28,
	9,
	3,
	8,
	37,
	36,
	36,
	14,
	36,
	36,
	14,
	14,
	36,
	36,
	36,
	36,
	36,
	36,
	14,
	21,
	14,
	14,
	36,
	36,
	36,
	36,
	6,
	14,
	21,
	0,
	0,
	3,
	21,
	1,
	0,
	21,
	3,
	29,
	14,
	2,
	32,
	10,
	18,
	21,
	1,
	0,
	3,
	14,
	14,
	11,
	35,
	14,
	36,
	36,
	3,
	36,
	33,
	33,
	1,
	26,
	10,
	24,
	25,
	24,
	30,
	2,
	3,
	3,
	14,
	2,
	14,
	5,
	16,
	25,
	37,
	9,
	23,
	11,
	35,
	2,
	1,
	33,
	26,
	35,
	3,
	3,
	0,
	35,
	2,
	2,
	35,
	1,
	1,
	1,
	33,
	35,
	31,
	33,
	1,
	13,
	29,
	11,
	18,
	0,
	3,
	1,
	3,
	0,
	32,
	1,
	18,
	2,
	21,
	9,
	1,
	1,
	33,
	27,
	10,
	12,
	12,
	21,
	31,
	21,
	22,
	12,
	6,
	26,
	36,
	3,
	3,
	30,
	24,
	3,
	18,
	12,
	30,
	10,
	1,
	10,
	1,
	3,
	21,
	3,
	10,
	10,
	1,
	1,
	37,
	13,
	10,
	6,
	37,
	1,
	1,
	1,
	1,
	1,
	35,
	37,
	10,
	28,
	26,
	23,
	18,
	20,
	26,
	10,
	10,
	0,
	36,
	1,
	13,
	12,
	33,
	18,
	2,
	2,
	1,
	3,
	1,
	25,
	29,
	25,
	27,
	33,
	18,
	10,
	15,
	29,
	29,
	14,
	14,
	14,
	13,
	3,
	28,
	34,
	26,
	29,
	28,
	27,
	9,
	9,
	30,
	32,
	37,
	6,
	37,
	6,
	37,
	32,
	11,
	35,
	33,
	15,
	35,
	37,
	29,
	35,
	1,
	1,
	37,
	1,
	1,
	0,
	37,
	15,
	18,
	1,
	5,
	35,
	27,
	2,
	15,
	37,
	1,
	11,
	35,
	33,
	37,
	26,
	33,
	1,
	35,
	37,
	0,
	0,
	10,
	1,
	1,
	11,
	1,
	0,
	35,
	1,
	3,
	1,
	18,
	0,
	1,
	29,
	1,
	10,
	6,
	10,
	3,
	3,
	21,
	10,
	28,
	27,
	22,
	27,
	15,
	24,
	0,
	13,
	12,
	2,
	37,
	1,
	18,
	28,
	37,
	1,
	22,
	13,
	1,
	8,
	15,
	27,
	1,
	1,
	28,
	1,
	31,
	12,
	1,
	1,
	3,
	1,
	1,
	35,
	1,
	2,
	33,
	11,
	2,
	0,
	1,
	2,
	0,
	2,
	1,
	33,
	1,
	0,
	1,
	26,
	13,
	9,
	20,
	14,
	1,
	32,
	24,
	6,
	33,
	2,
	15,
	32,
	31,
	9,
	6,
	18,
	21,
	27,
	35,
	1,
	18,
	35,
	35,
	35,
	38,
	38,
	24,
	38,
	21,
	12,
	3,
	3,
	14,
	14,
	30,
	25,
	30,
	3,
	14,
	1,
	14,
	24,
	25,
	1,
	14,
	15,
	30,
	24,
	24,
	21,
	36,
	30,
	34,
	5,
	36,
	23,
	37,
	32,
	8,
	24,
	21,
	24,
	26,
	34,
	28,
	24,
	9,
	21,
	32,
	27,
	21,
	15,
	24,
	28,
	37,
	38,
	5,
	35,
	37,
	32,
	11,
	32,
	11,
	37,
	35,
	20,
	11,
	11,
	37,
	32,
	32,
	37,
	32,
	32,
	33,
	27,
	9,
	18,
	12,
	35,
	2,
	24,
	30,
	29,
	18,
	35,
	37,
	36,
	27,
	34,
	21,
	31,
	29,
	30,
	29,
	31,
	7,
	24,
	1,
	9,
	3,
	3,
	9,
	9,
	34,
	27,
	0,
	26,
	18,
	18,
	18,
	18,
	35,
	18,
	35,
	11,
	0,
	0,
	9,
	9,
	34,
	34,
	14,
	10,
	3,
	24,
	30,
	30,
	2,
	32,
	1,
	18,
	37,
	26,
	28,
	35,
	25,
	35,
	17,
	29,
	14,
	27,
	37,
	15,
	15,
	0,
	24,
	13,
	12,
	18,
	1,
	1,
	3,
	0,
	3,
	10,
	10,
	30,
	9,
	37,
	30,
	27,
	3,
	16,
	25,
	2,
	32,
	10,
	10,
	10,
	10,
	18,
	33,
	3,
	3,
	1,
	2,
	0,
	2,
	1,
	28,
	1,
	0,
	29,
	29,
	11,
	29,
	2,
	30,
	2,
	34,
	32,
	29,
	9,
	29,
	1,
	1,
	35,
	29,
	32,
	1,
	0,
	9,
	1,
	15,
	1,
	30,
	13,
	0,
	9,
	1,
	18,
	18,
	1,
	1,
	0,
	1,
	1,
	1,
	18,
	18,
	29,
	33,
	29,
	14,
	1,
	1,
	2,
	2,
	3,
	32,
	1,
	11,
	18,
	21,
	7,
	24,
	24,
	21,
	21,
	18,
	18,
	1,
	1,
	18,
	16,
	37,
	30,
	24,
	18,
	18,
	1,
	29,
	29,
	9,
	10,
	32,
	14,
	3,
	3,
	3,
	3,
	3,
	1,
	1,
	1,
	13,
	1,
	0,
	0,
	35,
	37,
	29,
	35,
	2,
	3,
	35,
	2,
	3,
	3,
	2,
	1,
	2,
	35,
	1,
	25,
	35,
	0,
	26,
	35,
	32,
	2,
	18,
	0,
	0,
	16,
	11,
	0,
	1,
	1,
	1,
	1,
	33,
	0,
	1,
	35,
	33,
	30,
	29,
	15,
	33,
	29,
	1,
	13,
	18,
	35,
	1,
	27,
	24,
	31,
	1,
	14,
	14,
	26,
	36,
	36,
	15,
	16,
	2,
	9,
	1,
	18,
	18,
	18,
	0,
	0,
	18,
	13,
	18,
	29,
	11,
	11,
	6,
	37,
	1,
	7,
	6,
	26,
	18,
	13,
	11,
	1,
	22,
	33,
	18,
	30,
	5,
	2,
	16,
	14,
	6,
	28,
	16,
	9,
	33,
	2,
	25,
	10,
	2,
	36,
	2,
	12,
	0,
	29,
	1,
	11,
	0,
	9,
	18,
	0,
	33,
	37,
	1,
	38,
	18,
	2,
	2,
	32,
	10,
	0,
	6,
	18,
	1,
	22,
	35,
	11,
	20,
	9,
	21,
	10,
	0,
	29,
	18,
	14,
	36,
	9,
	9,
	9,
	33,
	24,
	24,
	32,
	24,
	24,
	33,
	24,
	36,
	35,
	28,
	32,
	37,
	35,
	24,
	26,
	24,
	24,
	18,
	3,
	3,
	18,
	36,
	2,
	33,
	0,
	29,
	1,
	1,
	1,
	12,
	0,
	16,
	2,
	32,
	22,
	36,
	30,
	9,
	30,
	36,
	24,
	29,
	24,
	24,
	24,
	9,
	36,
	9,
	29,
	1,
	1,
	11,
	37,
	35,
	0,
	15,
	26,
	36,
	8,
	29,
	1,
	0,
	1,
	12,
	27,
	0,
	35,
	35,
	0,
	1,
	28,
	29,
	24,
	1,
	1,
	1,
	0,
	1,
	18,
	1,
	1,
	1,
	1,
	18,
	29,
	18,
	18,
	18,
	0,
	1,
	9,
	29,
	1,
	1,
	29,
	11,
	33,
	18,
	35,
	35,
	11,
	1,
	11,
	18,
	18,
	35,
	18,
	18,
	11,
	11,
	18,
	1,
	1,
	30,
	9,
	14,
	26,
	35,
	37,
	2,
	29,
	33,
	8,
	35,
	11,
	29,
	25,
	32,
	21,
	29,
	2,
	18,
	33,
	29,
	18,
	35,
	1,
	35,
	1,
	6,
	2,
	32,
	21,
	0,
	0,
	1,
	0,
	18,
	35,
	18,
	1,
	35,
	29,
	35,
	18,
	18,
	29,
	35,
	18,
	11,
	35,
	25,
	1,
	3,
	1,
	14,
	24,
	9,
	14,
	9,
	34,
	36,
	34,
	34,
	36,
	22,
	3,
	9,
	1,
	9,
	14,
	34,
	9,
	9,
	36,
	36,
	36,
	9,
	1,
	29,
	1,
	33,
	37,
	37,
	35,
	5,
	14,
	32,
	9,
	29,
	35,
	29,
	26,
	13,
	22,
	32,
	7,
	14,
	24,
	24,
	1,
	35,
	13,
	9,
	0,
	18,
	35,
	11,
	27,
	14,
	24,
	24,
	30,
	14,
	24,
	24,
	14,
	14,
	14,
	24,
	18,
	14,
	14,
	14,
	14,
	14,
	17,
	17,
	1,
	3,
	0,
	2,
	21,
	1,
	1,
	1,
	2,
	3,
	3,
	2,
	3,
	0,
	3,
	1,
	3,
	3,
	3,
	3,
	1,
	3,
	3,
	3,
	3,
	3,
	3,
	16,
	0,
	3,
	34,
	11,
	17,
	17,
	14,
	34,
	14,
	34,
	14,
	11,
	14,
	30,
	17,
	11,
	17,
	34,
	1,
	34,
	9,
	17,
	17,
	14,
	34,
	18,
	18,
	35,
	35,
	35,
	18,
	35,
	18,
	18,
	29,
	25,
	1,
	1,
	13,
	0,
	2,
	2,
	2,
	13,
	13,
	3,
	35,
	1,
	29,
	35,
	2,
	35,
	29,
	18,
	18,
	18,
	18,
	11,
	25,
	18,
	11,
	23,
	33,
	0,
	0,
	3,
	3,
	9,
	16,
	1,
	12,
	0,
	2,
	35,
	33,
	1,
	27,
	0,
	1,
	1,
	1,
	14,
	1,
	1,
	1,
	9,
	1,
	30,
	14,
	14,
	23,
	11,
	16,
	27,
	0,
	1,
	9,
	31,
	12,
	28,
	31,
	5,
	24,
	15,
	26,
	12,
	22,
	6,
	26,
	32,
	14,
	15,
	9,
	3,
	33,
	3,
	27,
	2,
	9,
	1,
	1,
	37,
	32,
	15,
	14,
	21,
	1,
	0,
	3,
	0,
	26,
	26,
	9,
	22,
	30,
	34,
	14,
	1,
	14,
	14,
	29,
	30,
	30,
	36,
	1,
	1,
	14,
	27,
	14,
	33,
	30,
	1,
	30,
	14,
	1,
	6,
	7,
	21,
	37,
	33,
	3,
	3,
	22,
	2,
	25,
	29,
	33,
	16,
	14,
	14,
	4,
	17,
	14,
	25,
	24,
	34,
	14,
	14,
	14,
	1,
	14,
	3,
	2,
	2,
	3,
	3,
	14,
	14,
	2,
	3,
	14,
	14,
	3,
	16,
	35,
	18,
	11,
	29,
	1,
	27,
	12,
	35,
	33,
	1,
	18,
	11,
	16,
	16,
	16,
	16,
	16,
	16,
	16,
	16,
	32,
	16,
	9,
	11,
	10,
	10,
	32,
	9,
	16,
	14,
	16,
	9,
	13,
	10,
	13,
	15,
	9,
	9,
	32,
	6,
	23,
	29,
	29,
	11,
	12,
	20,
	0,
	1,
	1,
	1,
	1,
	0,
	1,
	0,
	11,
	36,
	29,
	21,
	9,
	29,
	34,
	2,
	27,
	18,
	11,
	34,
	9,
	37,
	10,
	11,
	11,
	3,
	12,
	2,
	27,
	10,
	10,
	1,
	2,
	1,
	16,
	10
 };
const vector<double> divisorsCC = { 
	2.58858,
	0.0624905,
	1,
	12.111,
	6.75077e+07,
	6.75318e+07,
	9.17493e+07,
	1,
	1.37413e+07,
	1.46616e+10,
	1.3211e+08,
	4.46472e+07,
	4.74661e+200,
	4.45383e+07,
	1,
	0.0328947,
	7.33019e+08,
	3.12392e+07,
	270000,
	1.56272e+08,
	270000,
	4.29243e+07,
	12.8556,
	1.49614e+10,
	4.4674e+07,
	3.20522e+06,
	2.08226e+193,
	1.24507e+27,
	4.4674e+07,
	4.4674e+07,
	1e+201,
	12802,
	4.64141e+07,
	6.3156,
	1.54705e+07,
	5.72977,
	1.47947e+60,
	0.75875,
	545.644,
	2.37819,
	1,
	1.45821e+07,
	20.7869,
	0.995,
	0.0175439,
	0.0501029,
	4.1085,
	3.74426,
	4572.31,
	1e+201,
	1.1175e+07,
	4.28018e+14,
	270000,
	5.32099,
	6.44282,
	1.9998e+06,
	1e+201,
	1e+201,
	1,
	0.82,
	0.9925,
	0.5,
	5.43686,
	0.5,
	1.98534e+06,
	5.30004,
	1.28039e+07,
	181.344,
	0.333333,
	0.0156,
	3.92538e+06,
	544.409,
	1.98716e+06,
	3.97432e+06,
	1e+201,
	5.15826,
	1.43649e+06,
	5.78269e+06,
	1e+201,
	7.3929e+11,
	0.0112999,
	2.99002,
	1,
	1e+201,
	1e+201,
	319248,
	1e+201,
	628092,
	847770,
	0.25,
	2.9238e+06,
	22936.6,
	434778,
	1e+201,
	1.40837e+07,
	1.35743,
	1.19913e+06,
	742661,
	1361.68,
	376026,
	3.39724,
	0.995,
	228944,
	0.5,
	0.5,
	0.333333,
	376026,
	752054,
	752054,
	59.3957,
	0.25,
	70,
	1,
	112589,
	22900,
	270000,
	270000,
	109054,
	1,
	5101
 };
const vector<vector<double> > featuresCC = {};