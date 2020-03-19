#include <OsiSolverInterface.hpp>
#include <limits>
#include <algorithm>
#include <cmath>
#include "OsiFeatures.hpp"

using namespace std;

int OsiFeatures::n = OFCount;

bool dbl_equal( const double v1, const double v2 ) {
    return fabs(v1-v2) <= 1e-16;
}

bool intVal( const double val ) {
    double iv = round(val + 0.5);
    return fabs(val - iv) <= 1e-16;
}

// max size for features
#define STR_SIZE 64

class Summary {
    public:
        Summary() :
            minV( numeric_limits<double>::max() ),
            maxV( numeric_limits<double>::min() ),
            minAbsV( numeric_limits<double>::max() ),
            maxAbsV( numeric_limits<double>::min() ),
            avg(0.0),
            ratioLSA(0.0),
            percIntEl(0.0),
            summV( 0.0 ),
            nEl(0), 
            intEl(0),
            allIntEl(0),
            nPosVal(0),
            nNegVal(0)
    { }

        void add( double val ) {
            this->minV = min(val, minV);
            this->maxV = max(val, maxV);
            double absv = abs(val);
            if (absv >= 1e-16) {
                this->minAbsV = min(absv, minAbsV);
                this->maxAbsV = max(absv, maxAbsV);
            }
            this->summV += val;
            if (intVal(val))
                this->intEl++;
            if (val >= 1e-16)
                this->nPosVal++;
            else {
                if (val <= -1e-16)
                    this->nNegVal++;
            }

            this->nEl++;
        }

        void finish() {
            this->avg = this->summV / ((long double) this->nEl);
            if (this->minAbsV != 0.0)
                this->ratioLSA = this->maxAbsV / this->minAbsV;

            if (this->intEl == this->nEl)
                this->allIntEl = 1;
            else
                this->allIntEl = 0;

            this->percIntEl = (((double)this->intEl) / ((double) this->nEl)) * 100.0;
        }

        double minV;
        double maxV;
        double minAbsV;
        double maxAbsV;
        double avg;
        double ratioLSA;
        double percIntEl;
        long double summV;
        unsigned int nEl;
        unsigned int intEl;
        unsigned int allIntEl;
        unsigned int nPosVal;
        unsigned int nNegVal;
};

const static char feat_names[OsiFeature::OFCount][STR_SIZE] = {
    "cols",
    "rows",
    "colsPerRow",
    "equalities",
    "percEqualities",
    "inequalities",
    "nz",
    "density",

    "bin",
    "genInt",
    "integer",
    "continuous",
    "percInteger",
    "percBin",

    "rPartitioning",
    "rPercPartitioning",
    "rPacking",
    "rPercPacking",
    "rCovering",
    "rPercCovering",
    "rCardinality",
    "rPercCardinality",
    "rKnapsack",
    "rPercKnapsack",
    "rIntegerKnapsack",
    "rPercIntegerKnapsack",
    "rInvKnapsack",
    "rPercInvKnapsack",
    "rSingleton",
    "rPercSingleton",
    "rAggre",
    "rPercAggre",
    "rPrec",
    "rPercPrec",
    "rVarBnd",
    "rPercVarBnd",
    "rBinPacking",
    "rPercBinPacking",
    "rMixedBin",
    "rPercMixedBin",
    "rGenInt",
    "rPercGenInt",
    "rFlowBin",
    "rPercFlowBin",
    "rFlowMx",
    "rPercFlowMx",

    "aMin",
    "aMax",
    "aAvg",
    "aStdDev",
    "aRatioLSA",
    "aAllInt",
    "aPercInt",

    "objMin",  
    "objMax",
    "objAvg",
    "objStdDev",
    "objRatioLSA",
    "objAllInt",
    "objPercInt",

    "rhsMin",
    "rhsMax",
    "rhsAvg",
    "rhsStdDev",
    "rhsRatioLSA",
    "rhsAllInt",
    "rhsPercInt",

    "rowNzMin",
    "rowNzMax",
    "rowNzAvg",
    "rowNzStdDev",

    "colNzMin",
    "colNzMax",
    "colNzAvg",
    "colNzStdDev",

    "rowsLessE4Nz",
    "rowsLessE8Nz",
    "rowsLessE16Nz",
    "rowsLessE32Nz",
    "rowsLessE64Nz",
    "rowsLessE128Nz",
    "rowsLessE256Nz",
    "rowsLessE512Nz",
    "rowsLessE1024Nz",
    "percRowsLessE4Nz",
    "percRowsLessE8Nz",
    "percRowsLessE16Nz",
    "percRowsLessE32Nz",
    "percRowsLessE64Nz",
    "percRowsLessE128Nz",
    "percRowsLessE256Nz",
    "percRowsLessE512Nz",
    "percRowsLessE1024Nz",

    "rowsLeast4Nz",
    "rowsLeast8Nz",
    "rowsLeast16Nz",
    "rowsLeast32Nz",
    "rowsLeast64Nz",
    "rowsLeast128Nz",
    "rowsLeast256Nz",
    "rowsLeast512Nz",
    "rowsLeast1024Nz",
    "rowsLeast2048Nz",
    "rowsLeast4096Nz",
    "percRowsLeast4Nz",
    "percRowsLeast8Nz",
    "percRowsLeast16Nz",
    "percRowsLeast32Nz",
    "percRowsLeast64Nz",
    "percRowsLeast128Nz",
    "percRowsLeast256Nz",
    "percRowsLeast512Nz",
    "percRowsLeast1024Nz",
    "percRowsLeast2048Nz",
    "percRowsLeast4096Nz",

    "colsLessE4Nz",
    "colsLessE8Nz",
    "colsLessE16Nz",
    "colsLessE32Nz",
    "colsLessE64Nz",
    "colsLessE128Nz",
    "colsLessE256Nz",
    "colsLessE512Nz",
    "colsLessE1024Nz",
    "percColsLessE4Nz",
    "percColsLessE8Nz",
    "percColsLessE16Nz",
    "percColsLessE32Nz",
    "percColsLessE64Nz",
    "percColsLessE128Nz",
    "percColsLessE256Nz",
    "percColsLessE512Nz",
    "percColsLessE1024Nz",

    "colsLeast4Nz",
    "colsLeast8Nz",
    "colsLeast16Nz",
    "colsLeast32Nz",
    "colsLeast64Nz",
    "colsLeast128Nz",
    "colsLeast256Nz",
    "colsLeast512Nz",
    "colsLeast1024Nz",
    "colsLeast2048Nz",
    "colsLeast4096Nz"
    "percColsLeast4Nz",
    "percColsLeast8Nz",
    "colsLeast16Nz",
    "colsLeast32Nz",
    "colsLeast64Nz",
    "colsLeast128Nz",
    "colsLeast256Nz",
    "colsLeast512Nz",
    "colsLeast1024Nz",
    "colsLeast2048Nz",
    "colsLeast4096Nz",
};

const char *OsiFeatures::name( int i ) {
    return feat_names[i];
}

const char *OsiFeatures::name( const OsiFeature of ) {
    return OsiFeatures::name((int) of);
}

double std_dev( const int *el, const double avg, int n ) {
    long double sum = 0.0;

    for ( int i=0 ; (i<n) ; ++i ) {
        const double diff = (((double)el[i])-avg);
        sum += diff * diff;
    }
    sum /= ((long double)n);

    return (double) sqrtl(sum);
}



double std_dev( const double *el, const double avg, int n ) {
    long double sum = 0.0;

    for ( int i=0 ; (i<n) ; ++i ) {
        const double diff = (el[i]-avg);
        sum += diff * diff;
    }
    sum /= ((long double)n);

    return (double) sqrtl(sum);
}

void OsiFeatures::compute(double *features, OsiSolverInterface *solver) {
    // initializing
    for ( int i=0 ; i<OsiFeatures::n ; ++i  )
        features[i] = 0.0;

    features[OFcols] = solver->getNumCols();
    features[OFrows] = solver->getNumRows();
    features[OFcolsPerRow] = ((double)(solver->getNumCols())) / ((double)(solver->getNumRows()));
    features[OFnz] = solver->getNumElements();
    features[OFdensity] = (((long double)solver->getNumElements()) / (((long double)solver->getNumCols())*((long double)solver->getNumRows()))) * ((long double) 100.0);

    Summary aSumm, rhsSumm, objSumm, rowNzSumm, colNzSumm;

    /* going though all rows */
    unsigned int nRows = solver->getNumRows();
    const double *rhs = solver->getRightHandSide();
    const char *sense = solver->getRowSense();
    const CoinPackedMatrix *cpmRow =  solver->getMatrixByRow();
    for ( unsigned int row=0 ; (row<nRows) ; ++row ) {
        const int nzRow = cpmRow->getVectorLengths()[row];
        const CoinBigIndex *starts = cpmRow->getVectorStarts();
        const int *ridx = cpmRow->getIndices() + starts[row];
        const double *rcoef = cpmRow->getElements() + starts[row];

        rowNzSumm.add(nzRow);

        Summary summRow;

        int nBinRow = 0, nContRow = 0, nIntRow = 0;
        for (int j = 0 ; (j < nzRow) ; ++j) {
            aSumm.add(rcoef[j]);
            summRow.add(rcoef[j]);

            if (solver->isBinary(ridx[j]))
                nBinRow++;
            else
            {
                if (solver->isInteger(ridx[j]))
                    nIntRow++;
                else
                    nContRow++;
            }
        }

        bool rowBin = (nBinRow == nzRow);
        bool intCoefs = summRow.allIntEl;

        switch (nzRow) {
            case 1:
                features[OFrowsSingleton]++;
                break;
            case 2:
                if (sense[row] == 'E')
                    features[OFrowsAggr]++;

                if (nBinRow == 1)
                    features[OFrowsVarBnd]++;

                if ( nBinRow%2 == 0 && nContRow%2 == 0) // vars of the same type
                    if ( summRow.nNegVal == 1 && summRow.nPosVal == 1
                            && dbl_equal(summRow.minV, summRow.maxV) )
                        features[OFrowsPrec]++;
                break;
        }

        // constraint types with only binary variables
        const double minaRow = summRow.minV;
        const double maxaRow = summRow.maxV;

        if (rowBin) {
            // pack, part and cov
            if ( dbl_equal(minaRow, 1.0) && dbl_equal(maxaRow, 1.0)  ) {
                if (dbl_equal(rhs[row], 1.0)) {
                    switch (sense[row]) {
                        case 'E':
                            features[OFrowsPartitioning]++;
                            break;
                        case 'G':
                            features[OFrowsCovering]++;
                            break;
                        case 'L':
                            features[OFrowsPacking]++;
                            break;
                    }
                } // rhs 1.0
                else {
                    if (rhs[row] >= 1.99) {
                        switch (sense[row]) {
                            case 'E':
                                features[OFrowsCardinality]++;
                                break;
                            case 'L':
                                features[OFrowsInvKnapsack]++;
                                break;
                        }
                    } // rhs >= 2
                } // rhs != 1
            } // only ones LHS as coefs
            else {
                if (rhs[row] >= 1.1 ) {
                    if ( (maxaRow - minaRow >= 0.1) && (summRow.nNegVal == 0) ) { // different weights
                        features[OFrowsKnapsack]++;
                        if (intCoefs) {
                            features[OFrowsIntegerKnapsack]++;
                        }
                    }
                    if (summRow.nNegVal == 1 && nzRow >= 2) {
                        features[OFrowsBinPacking]++;
                    }
                }
            }

            if (summRow.nNegVal >= 2 && summRow.nPosVal >= 2 && sense[row]=='E')
                features[OFrowsFlowBin]++;
        } // only binary vars
        else
        {
            if (summRow.nNegVal >= 2 && summRow.nPosVal >= 2 && sense[row]=='E')
                features[OFrowsFlowMx]++;
            if (nContRow>0)
                features[OFrowsMixedBin]++;
            if (nIntRow)
                features[OFrowsGenInt]++;
        }

        switch (sense[row]) {
            case 'E':
                features[OFequalities]++;
                break;
            default:
                // inequalities
                features[OFinequalities]++;
                break;
        }

        rhsSumm.add( rhs[row] );

        if (nzRow <= 1024) {
            features[OFrowsLess1024Nz]++;
            if (nzRow <= 512) {
                features[OFrowsLess512Nz]++;
                if (nzRow <= 256) {
                    features[OFrowsLess256Nz]++;
                    if (nzRow <= 128) {
                        features[OFrowsLess128Nz]++;
                        if (nzRow <= 64) {
                            features[OFrowsLess64Nz]++;
                            if (nzRow <= 32) {
                                features[OFrowsLess32Nz]++;
                                if (nzRow <= 16) {
                                    features[OFrowsLess16Nz]++;
                                    if (nzRow <= 8) {
                                        features[OFrowsLess8Nz]++;
                                        if (nzRow <= 4) {
                                            features[OFrowsLess4Nz]++;
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        } // checking small rows

        if (nzRow >= 4) {
            features[OFrowsLeast4Nz]++;
            if (nzRow >= 8) {
                features[OFrowsLeast8Nz]++;
                if (nzRow >= 16) {
                    features[OFrowsLeast16Nz]++;
                    if (nzRow >= 32) {
                        features[OFrowsLeast32Nz]++;
                        if (nzRow >= 64) {
                            features[OFrowsLeast64Nz]++;
                            if (nzRow >= 128) {
                                features[OFrowsLeast128Nz]++;
                                if (nzRow >= 256) {
                                    features[OFrowsLeast256Nz]++;
                                    if (nzRow >= 512) {
                                        features[OFrowsLeast512Nz]++;
                                        if (nzRow >= 1024) {
                                            features[OFrowsLeast1024Nz]++;
                                            if (nzRow >= 2048) {
                                                features[OFrowsLeast2048Nz]++;
                                                if (nzRow >= 4096) {
                                                    features[OFrowsLeast4096Nz]++;
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            } 
        } // larger rows
    } // rows
    
    features[OFpercRowsPartitioning] = (features[OFrowsPartitioning] / (double)solver->getNumRows())*100.0;
    features[OFpercRowsPacking] = (features[OFrowsPacking] / (double)solver->getNumRows())*100.0;
    features[OFpercRowsCovering] = (features[OFrowsCovering] / (double)solver->getNumRows())*100.0;
    features[OFpercRowsCardinality] = (features[OFrowsCardinality] / (double)solver->getNumRows())*100.0;
    features[OFpercRowsIntegerKnapsack] = (features[OFrowsIntegerKnapsack] / (double)solver->getNumRows())*100.0;
    features[OFpercRowsInvKnapsack] = (features[OFrowsInvKnapsack] / (double)solver->getNumRows())*100.0;
    features[OFpercRowsSingleton] = (features[OFrowsSingleton] / (double)solver->getNumRows())*100.0;
    features[OFpercRowsAggr] = (features[OFrowsAggr] / (double)solver->getNumRows())*100.0;
    features[OFpercRowsPrec] = (features[OFrowsPrec] / (double)solver->getNumRows())*100.0;
    features[OFpercRowsVarBnd] = (features[OFrowsVarBnd] / (double)solver->getNumRows())*100.0;
    features[OFpercRowsBinPacking] = (features[OFrowsBinPacking] / (double)solver->getNumRows())*100.0;
    features[OFpercRowsMixedBin] = (features[OFrowsMixedBin] / (double)solver->getNumRows())*100.0;
    features[OFpercRowsGenInt] = (features[OFrowsGenInt] / (double)solver->getNumRows())*100.0;
    features[OFpercRowsFlowBin] = (features[OFpercRowsFlowBin] / (double)solver->getNumRows())*100.0;
    features[OFpercRowsFlowMx] = (features[OFpercRowsFlowMx] / (double)solver->getNumRows())*100.0;
    aSumm.finish();
    rhsSumm.finish();
    rowNzSumm.finish();

    // cols
    const CoinPackedMatrix *cpmCol =  solver->getMatrixByCol();
    const double *obj = solver->getObjCoefficients();
    for ( int j=0 ; (j<solver->getNumCols()) ; ++j ) {
        objSumm.add(obj[j]);

        if (solver->isInteger(j)) {
            if (solver->isBinary(j))
                features[OFbin]++;
            else
                features[OFgenInt]++;

            features[OFinteger]++;
        }
        else
            features[OFcontinuous]++;

        const int nzCol = cpmCol->getVectorLengths()[j];

        colNzSumm.add(nzCol);
        //const CoinBigIndex *starts = cpmCol->getVectorStarts();
        //const int *ridx = cpmCol->getIndices() + starts[j];
        //const double *rcoef = cpmCol->getElements() + starts[j];

        if (nzCol <= 1024) {
            features[OFcolsLess1024Nz]++;
            if (nzCol <= 512) {
                features[OFcolsLess512Nz]++;
                if (nzCol <= 256) {
                    features[OFcolsLess256Nz]++;
                    if (nzCol <= 128) {
                        features[OFcolsLess128Nz]++;
                        if (nzCol <= 64) {
                            features[OFcolsLess64Nz]++;
                            if (nzCol <= 32) {
                                features[OFcolsLess32Nz]++;
                                if (nzCol <= 16) {
                                    features[OFcolsLess16Nz]++;
                                    if (nzCol <= 8) {
                                        features[OFcolsLess8Nz]++;
                                        if (nzCol <= 4) {
                                            features[OFcolsLess4Nz]++;
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        } // checking small cols

        if (nzCol >= 4) {
            features[OFcolsLeast4Nz]++;
            if (nzCol >= 8) {
                features[OFcolsLeast8Nz]++;
                if (nzCol >= 16) {
                    features[OFcolsLeast16Nz]++;
                    if (nzCol >= 32) {
                        features[OFcolsLeast32Nz]++;
                        if (nzCol >= 64) {
                            features[OFcolsLeast64Nz]++;
                            if (nzCol >= 128) {
                                features[OFcolsLeast128Nz]++;
                                if (nzCol >= 256) {
                                    features[OFcolsLeast256Nz]++;
                                    if (nzCol >= 512) {
                                        features[OFcolsLeast512Nz]++;
                                        if (nzCol >= 1024) {
                                            features[OFcolsLeast1024Nz]++;
                                            if (nzCol >= 2048) {
                                                features[OFcolsLeast2048Nz]++;
                                                if (nzCol >= 4096) {
                                                    features[OFcolsLeast4096Nz]++;
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            } 
        } // larger cols

    } // all variables
    objSumm.finish();
    features[OFpercInteger] = (features[OFinteger] / ((double) solver->getNumCols())) * 100.0;
    features[OFpercBin] = (features[OFbin] / ((double) solver->getNumCols())) * 100.0;

    features[OFaMin] = aSumm.minV;
    features[OFaMax] = aSumm.maxV;
    features[OFaAvg] = aSumm.avg;
    features[OFaStdDev] = std_dev( cpmRow->getElements(), aSumm.avg, solver->getNumElements() );
    features[OFaRatioLSA] = aSumm.ratioLSA;
    features[OFaAllInt] = aSumm.allIntEl;
    features[OFaPercInt] = aSumm.percIntEl;

    features[OFobjMin] = objSumm.minV;
    features[OFobjMax] = objSumm.maxV;
    features[OFobjAvg] = objSumm.avg;
    features[OFobjStdDev] = std_dev( solver->getObjCoefficients(), objSumm.avg, solver->getNumCols() );
    features[OFobjRatioLSA] = objSumm.ratioLSA;
    features[OFobjAllInt] = objSumm.allIntEl;
    features[OFobjPercInt] = objSumm.percIntEl;

    features[OFrhsMin] = rhsSumm.minV;
    features[OFrhsMax] = rhsSumm.maxV;
    features[OFrhsAvg] = rhsSumm.avg;
    features[OFrhsStdDev] = std_dev( solver->getRightHandSide(), rhsSumm.avg, solver->getNumRows() );
    features[OFrhsRatioLSA] = rhsSumm.ratioLSA;
    features[OFrhsAllInt] = rhsSumm.allIntEl;
    features[OFrhsPercInt] = rhsSumm.percIntEl;

    features[OFrowNzMin] = rowNzSumm.minV;
    features[OFrowNzMax] = rowNzSumm.maxV;
    features[OFrowNzAvg] = rowNzSumm.avg;
    features[OFrowNzStdDev] = std_dev(cpmRow->getVectorLengths(), rowNzSumm.avg, solver->getNumRows() );

    features[OFcolNzMin] = colNzSumm.minV;
    features[OFcolNzMax] = colNzSumm.maxV;
    features[OFcolNzAvg] = colNzSumm.avg;
    features[OFcolNzStdDev] = std_dev(cpmRow->getVectorLengths(), colNzSumm.avg, solver->getNumCols() );

    double dnRows = solver->getNumRows();
    features[OFpercRowsLess4Nz] = (features[OFrowsLess4Nz] / dnRows)*100.0;
    features[OFpercRowsLess8Nz] = (features[OFrowsLess8Nz] / dnRows)*100.0;
    features[OFpercRowsLess16Nz] = (features[OFrowsLess16Nz] / dnRows)*100.0;
    features[OFpercRowsLess32Nz] = (features[OFrowsLess32Nz] / dnRows)*100.0;
    features[OFpercRowsLess64Nz] = (features[OFrowsLess64Nz] / dnRows)*100.0;
    features[OFpercRowsLess128Nz] = (features[OFrowsLess128Nz] / dnRows)*100.0;
    features[OFpercRowsLess256Nz] = (features[OFrowsLess256Nz] / dnRows)*100.0;
    features[OFpercRowsLess512Nz] = (features[OFrowsLess512Nz] / dnRows)*100.0;
    features[OFpercRowsLess1024Nz] = (features[OFrowsLess1024Nz] / dnRows)*100.0;

    features[OFpercRowsLeast4Nz] = (features[OFrowsLeast4Nz] / dnRows)*100.0;
    features[OFpercRowsLeast8Nz] = (features[OFrowsLeast8Nz] / dnRows)*100.0;
    features[OFpercRowsLeast16Nz] = (features[OFrowsLeast16Nz] / dnRows)*100.0;
    features[OFpercRowsLeast32Nz] = (features[OFrowsLeast32Nz] / dnRows)*100.0;
    features[OFpercRowsLeast64Nz] = (features[OFrowsLeast64Nz] / dnRows)*100.0;
    features[OFpercRowsLeast128Nz] = (features[OFrowsLeast128Nz] / dnRows)*100.0;
    features[OFpercRowsLeast256Nz] = (features[OFrowsLeast256Nz] / dnRows)*100.0;
    features[OFpercRowsLeast512Nz] = (features[OFrowsLeast512Nz] / dnRows)*100.0;
    features[OFpercRowsLeast1024Nz] = (features[OFrowsLeast1024Nz] / dnRows)*100.0;
    features[OFpercRowsLeast2048Nz] = (features[OFrowsLeast2048Nz] / dnRows)*100.0;
    features[OFpercRowsLeast4096Nz] = (features[OFrowsLeast4096Nz] / dnRows)*100.0;

    double dnCols = solver->getNumCols();
    features[OFpercColsLess4Nz] = (features[OFcolsLess4Nz] / dnCols)*100.0;
    features[OFpercColsLess8Nz] = (features[OFcolsLess8Nz] / dnCols)*100.0;
    features[OFpercColsLess16Nz] = (features[OFcolsLess16Nz] / dnCols)*100.0;
    features[OFpercColsLess32Nz] = (features[OFcolsLess32Nz] / dnCols)*100.0;
    features[OFpercColsLess64Nz] = (features[OFcolsLess64Nz] / dnCols)*100.0;
    features[OFpercColsLess128Nz] = (features[OFcolsLess128Nz] / dnCols)*100.0;
    features[OFpercColsLess256Nz] = (features[OFcolsLess256Nz] / dnCols)*100.0;
    features[OFpercColsLess512Nz] = (features[OFcolsLess512Nz] / dnCols)*100.0;
    features[OFpercColsLess1024Nz] = (features[OFcolsLess1024Nz] / dnCols)*100.0;

    features[OFpercColsLeast4Nz] = (features[OFcolsLeast4Nz] / dnCols)*100.0;
    features[OFpercColsLeast8Nz] = (features[OFcolsLeast8Nz] / dnCols)*100.0;
    features[OFpercColsLeast16Nz] = (features[OFcolsLeast16Nz] / dnCols)*100.0;
    features[OFpercColsLeast32Nz] = (features[OFcolsLeast32Nz] / dnCols)*100.0;
    features[OFpercColsLeast64Nz] = (features[OFcolsLeast64Nz] / dnCols)*100.0;
    features[OFpercColsLeast128Nz] = (features[OFcolsLeast128Nz] / dnCols)*100.0;
    features[OFpercColsLeast256Nz] = (features[OFcolsLeast256Nz] / dnCols)*100.0;
    features[OFpercColsLeast512Nz] = (features[OFcolsLeast512Nz] / dnCols)*100.0;
    features[OFpercColsLeast1024Nz] = (features[OFcolsLeast1024Nz] / dnCols)*100.0;
    features[OFpercColsLeast2048Nz] = (features[OFcolsLeast2048Nz] / dnCols)*100.0;
    features[OFpercColsLeast4096Nz] = (features[OFcolsLeast4096Nz] / dnCols)*100.0;
}
