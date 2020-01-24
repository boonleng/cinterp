#include "wf.h"

#define WF_RADIAN(x)  (x / 180.0 * M_PI)
#define WF_DEGREE(x)  (x / M_PI * 180.0)

/* Some functions that aren't expected to be used outside */
int WFCompareCellIds(const void *v1,                  /* Value 1               */
                     const void *v2);                 /* Value 2               */

void WFSortRadarCellIds(WFRadarCellId *cellIds,       /* Cell indices          */
                        const size_t count);          /* Count of cell indices */

char WFRadarCellIdIsNewInSet(WFRadarCellId testId,    /* Test index            */
                             WFRadarCellId *cellIds,  /* Cell indices          */
                             const size_t count);     /* Count of cell indices */

#pragma mark -
#pragma mark Internal Functions

int WFCompareCellIds(const void *v1,                  /* Value 1 */
                     const void *v2)                  /* Value 2 */
{
    WFRadarCellId *c1 = (WFRadarCellId *)v1;
    WFRadarCellId *c2 = (WFRadarCellId *)v2;
    
    if (c1->azimuthId <  c2->azimuthId) return -1;
    if (c1->azimuthId == c2->azimuthId) {
        if (c1->rangeId <  c2->rangeId) return -1;
        if (c1->rangeId == c2->rangeId) return 0;
        return 1;
    }
    return 1;
}


void WFSortRadarCellIds(WFRadarCellId *cellIds,       /* Cell indices          */
                        const size_t count)           /* Count of cell indices */
{
    qsort(cellIds, count, sizeof(WFRadarCellId), WFCompareCellIds);
}


char WFRadarCellIdIsNewInSet(WFRadarCellId testId,    /* Test index            */
                             WFRadarCellId *cellIds,  /* Cell indices          */
                             const size_t count)      /* Count of cell indices */
{
    for (size_t j=0; j<count; j++) {
        if (testId.azimuthId   == cellIds[j].azimuthId  &&
            testId.rangeId     == cellIds[j].rangeId) {
            return 0;
        }
    }
    return 1;
}

#pragma mark -
#pragma mark Coordinate

WFCoordStruct WFCoordStructMake(double latitude,       /* Latitude  */
                                double longitude)      /* Longitude */
{
    WFCoordStruct coord = {latitude, longitude};
    return coord;
}

#pragma mark -
#pragma mark Configuration

WFConfigStruct *WFConfigInit(void)
{
    return WFConfigInitFromFile(WF_DEFAULT_CONFIG_FILE, 0);
}


WFConfigStruct *WFConfigInitFromFile(const char *file,  /* Input filename */
                                     const char verb)   /* Verbose level  */
{
    WFConfigStruct *C = (WFConfigStruct *)malloc(sizeof(WFConfigStruct));
    if (C == NULL) {
        fprintf(stderr, "%s : WF : Error initialing.\n", now());
    } else {
        /* Set everything to zero */
        memset(C, 0, sizeof(WFConfigStruct));
        
        /* Set some default values */
        C->paddingInAzimuthDegrees = WF_DEFAULT_PAD_AZ;
        C->paddingInRangeMeters = WF_DEFAULT_PAD_RANGE;
        
        /* Copy over the verb preference supplied. */
        C->verb = verb;
        
        /* Get some basic info of the file */
        int ret;
        struct stat fileStats;
        char filename[WF_STRLEN];
        if (strlen(file) == 0) {
            strncpy(filename, WF_DEFAULT_CONFIG_FILE, WF_STRLEN);
        } else {
            strncpy(filename, file, WF_STRLEN);
        }
        ret = stat(filename, &fileStats);
        if (ret < 0) {
            if (!strcmp(filename, WF_DEFAULT_CONFIG_FILE)) {
                fprintf(stderr, "\033[32mNo configuration file.\033[0m\n");
                printf("%s : WF : mCreating an example config file.\n", now());
                FILE *fid = fopen(WF_DEFAULT_CONFIG_FILE, "w");
                if (fid == NULL) {
                    fprintf(stderr, "%s : WF : Error creating config file.\n", now());
                    return NULL;
                }
                fprintf(fid, "#\n");
                fprintf(fid, "# Processing parameters\n");
                fprintf(fid, "#\n");
                fprintf(fid, "\n");
                fprintf(fid, "PAD_AZ_DEG    1.0\n");
                fprintf(fid, "PAD_RANGE_M   250.0\n");
                fprintf(fid, "MULTIPATH_M   250.0\n");
                fprintf(fid, "\n");
                fprintf(fid, "#\n");
                fprintf(fid, "# Wind turbine locations (latitude, longitude, multipath extension)\n");
                fprintf(fid, "#\n\n");
                fprintf(fid, "\n");
                fprintf(fid, "TURBINE_LOC   43.339592  -81.873163\n");
                fprintf(fid, "TURBINE_LOC   43.345848  -81.873892  1500\n");
                fprintf(fid, "TURBINE_LOC   43.352112  -81.874470\n");
                fprintf(fid, "TURBINE_LOC   43.358383  -81.874897\n");
                fprintf(fid, "TURBINE_LOC   43.345576  -81.878824\n");
                fprintf(fid, "TURBINE_LOC   43.351902  -81.879408\n");
                fprintf(fid, "TURBINE_LOC   43.364573  -81.880121  1200\n");
                fprintf(fid, "TURBINE_LOC   43.374084  -81.880257\n");
                fprintf(fid, "TURBINE_LOC   43.377255  -81.880226\n");
                fprintf(fid, "TURBINE_LOC   43.380425  -81.880157\n");
                fprintf(fid, "TURBINE_LOC   43.614683  -81.826850\n");
                fprintf(fid, "TURBINE_LOC   43.620310  -81.820952\n");
                fprintf(fid, "TURBINE_LOC   43.625860  -81.814919\n");
                fprintf(fid, "TURBINE_LOC   43.631333  -81.808751\n");
                fprintf(fid, "TURBINE_LOC   43.647272  -81.789463\n");
                fprintf(fid, "TURBINE_LOC   43.652419  -81.782778\n");
                fprintf(fid, "TURBINE_LOC   43.657480  -81.775969\n");
                fprintf(fid, "TURBINE_LOC   43.662454  -81.769038\n");
                fprintf(fid, "TURBINE_LOC   43.667340  -81.761987\n");
                fprintf(fid, "TURBINE_LOC   43.672135  -81.754818\n");
                fclose(fid);
                ret = stat(filename, &fileStats);
                if (ret < 0) {
                    printf("%s : WF : Inable to continue.\n", now());
                    return NULL;
                }
            } else {
                fprintf(stderr, "%s : WF : Configuration file does not exist.\n", now());
                return NULL;
            }
        }
        
        if (C->verb > 1) {
            printf("%s : WF : file size = %d B\n", now(), (int)fileStats.st_size);
        }
        
        char *buffer = (char *)malloc(fileStats.st_size + 1);
        FILE *fid = fopen(filename, "r");
        size_t bytesRead = fread(buffer, sizeof(char), fileStats.st_size, fid);
        fclose(fid);
        
        /* Double check the number of bytes */
        if (bytesRead != (size_t)fileStats.st_size) {
            fprintf(stderr, "Unexpected number of bytes. Expected %zu by read %zu\n", (size_t)fileStats.st_size, bytesRead);
        }
        
        /* Count number of lines and assume that's at most the number of wind turbines in the config file. */
        size_t n = 1;
        char *theChar = buffer;
        while (theChar < buffer + fileStats.st_size) {
            if (*theChar++ == '\n') {
                n++;
            }
        }
        if (C->verb  > 1) {
            printf("%s : WF : line count = %zu\n", now(), n);
        }
        
        C->turbineLocation = (WFCoordStruct *)malloc(n * sizeof(WFCoordStruct));
        C->multipathLength = (float *)malloc(n * sizeof(float));
        if (C->turbineLocation == NULL || C->multipathLength == NULL) {
            fprintf(stderr, "%s : WF : Error allocating memory for turbine locations.\n", now());
            return NULL;
        }
        memset(C->turbineLocation, 0, n * sizeof(WFCoordStruct));
        memset(C->multipathLength, 0, n * sizeof(float));
        
        /* Now, parse out the wind turbine locations */
        theChar = buffer;
        char *lineBegin = theChar;
        C->count = 0;
        char *name = (char *)malloc(256);
        while (theChar < buffer + fileStats.st_size) {
            if (*theChar == '\n' || theChar == buffer + fileStats.st_size - 1) {
                *theChar = '\0';
                /* Scan from *b, which is b[0] */
                switch (*lineBegin) {
                    case 'M':
                        if (!strncmp(lineBegin, "MULTIPATH", 9)) {
                            ret = sscanf(lineBegin, "%s %f",
                                         name,
                                         &C->globalMultipathLength);
                            if (ret < 2) {
                                fprintf(stderr, "WARNING: Global multipath setting incomplete.\n");
                            }
                        }
                        break;
                    case 'P':
                        if (!strncmp(lineBegin, "PAD_AZ", 6)) {
                            ret = sscanf(lineBegin, "%s %f", name, &C->paddingInAzimuthDegrees);
                            if (ret < 2) {
                                fprintf(stderr, "WARNING: Padding in azimuth setting incomplete.\n");
                            } else if (C->paddingInAzimuthDegrees > 5.0f) {
                                printf("%s : WF : INFO: Padding in azimuth = %.2f deg > 5.0 deg.\n", now(), C->paddingInAzimuthDegrees);
                            }
                        } else if (!strncmp(lineBegin, "PAD_RANGE", 9)) {
                            ret = sscanf(lineBegin, "%s %f",
                                         name,
                                         &C->paddingInRangeMeters);
                            if (ret < 2) {
                                fprintf(stderr, "WARNING: Padding in range setting incomplete.\n");
                            } else if (C->paddingInRangeMeters > 10000.0f) {
                                printf("%s : WF : INFO: Padding in range = %.1f m > 10000.0 m.\n", now(), C->paddingInRangeMeters);
                            }
                        }
                        break;
                    case 'T':
                        if (!strncmp(lineBegin, "TURBINE_LOC", 11)) {
                            size_t k = C->count;
                            ret = sscanf(lineBegin, "%s %lf %lf %f",
                                         name,
                                         &C->turbineLocation[k].latitude,
                                         &C->turbineLocation[k].longitude,
                                         &C->multipathLength[k]);
                            if (ret < 2) {
                                fprintf(stderr, "WARNING: Turbine [%zu] has an incomplete location.\n", k);
                            } else if (C->multipathLength[k] > 10000.0f) {
                                printf("%s : WF : INFO: Multipath extension [%zu] %.1f > 10000.0 m.\n", now(), k, C->multipathLength[k]);
                            }
                            C->count++;
                        }
                        break;
                    default:
                        break;
                }
                /* Next line begin at m + 1 */
                lineBegin = theChar + 1;
            }
            theChar++;
        }
        free(name);
        free(buffer);

        /* Present a summary */
        if (C->verb) {
            WFConfigShowSummary(C);
        }
        
    }
    return C;
}


void WFConfigRelease(WFConfigStruct *config)   /* Configuration structure */
{
    if (config->turbineLocation != NULL) {
        free(config->turbineLocation);
        free(config->multipathLength);
    } else {
        fprintf(stderr, "%s : WF : Releasing resource that has not be allocated.\n", now());
        return;
    }
    free(config);
}


void WFConfigShowSummary(WFConfigStruct *config)   /* Configuration structure */
{
    size_t i;
    printf("%s : WF : %zu turbines\n", now(), config->count);
    printf("%s : WF : Padding in AZ = %.1f deg\n", now(), config->paddingInAzimuthDegrees);
    printf("%s : WF : Padding in RANGE = %.1f m\n", now(), config->paddingInRangeMeters);
    printf("%s : WF : Global MULTIPATH = %.2f m\n", now(), config->globalMultipathLength);
    for (i=0; i<(config->count < 9 ? config->count : 5); i++) {
        printf("%s : WF :   Turbine %3zd - %10.6f, %11.6f  %6.1f m\n",
               now(), i, config->turbineLocation[i].latitude, config->turbineLocation[i].longitude, config->multipathLength[i]);
    }
    if (i <= config->count - 1) {
        printf("%s : WF :       :     : -    :      ,     :\n", now());
        printf("%s : WF :       :     : -    :      ,     :\n", now());
        i = config->count - 1;
        printf("%s : WF :   Turbine %3zd - %10.6f, %11.6f  %6.1f m\n",
               now(), i, config->turbineLocation[i].latitude, config->turbineLocation[i].longitude, config->multipathLength[i]);
    }
}


#pragma mark -
#pragma mark Coordinate Transformations

WFRadarCoordStruct WFGetRadarCoord(const WFCoordStruct targetCoord,   /* Target coordinate */
                                   const WFCoordStruct radarCoord)    /* Radar coordinate  */
{
    WFRadarCoordStruct coord;

    /* 3-D location of the given point */
    double targetX = WF_EARTH_RADIUS * cos(WF_RADIAN(targetCoord.latitude)) * sin(WF_RADIAN(targetCoord.longitude));
    double targetY = WF_EARTH_RADIUS * cos(WF_RADIAN(targetCoord.latitude)) * cos(WF_RADIAN(targetCoord.longitude));
    double targetZ = WF_EARTH_RADIUS * sin(WF_RADIAN(targetCoord.latitude));

    /* 3-D location of the radar */
    double radarX = WF_EARTH_RADIUS * cos(WF_RADIAN(radarCoord.latitude)) * sin(WF_RADIAN(radarCoord.longitude));
    double radarY = WF_EARTH_RADIUS * cos(WF_RADIAN(radarCoord.latitude)) * cos(WF_RADIAN(radarCoord.longitude));
    double radarZ = WF_EARTH_RADIUS * sin(WF_RADIAN(radarCoord.latitude));
    
    /* Deltas */
    double dx = targetX - radarX;
    double dy = targetY - radarY;
    double dz = targetZ - radarZ;
    
    coord.range = sqrt(dx * dx + dy * dy + dz * dz);
    
    coord.azimuth = atan2((targetCoord.longitude - radarCoord.longitude) * cos(radarCoord.latitude),
                          (targetCoord.latitude - radarCoord.latitude));
    
    coord.elevation = 0.0f;
    
    printf("%.8lf, %.8lf --> %.6lf, %.6lf\n", targetCoord.latitude, targetCoord.longitude, coord.range, WF_DEGREE(coord.azimuth));
    
    return coord;
}


void WFDeriveRadarCoords(WFRadarCoordStruct *coords,     /* Target's radar coordinates   */
                         const WFCoordStruct *targets,   /* Target's lat/lon coordinates */
                         const WFCoordStruct *origin,    /* Radar's lat/lon coordinate   */
                         size_t count)                   /* Number of elements           */
{
    /* 3-D location of the radar origin */
    double radarX = cos(WF_RADIAN(origin->latitude)) * sin(WF_RADIAN(origin->longitude));
    double radarY = cos(WF_RADIAN(origin->latitude)) * cos(WF_RADIAN(origin->longitude));
    double radarZ = sin(WF_RADIAN(origin->latitude));
    double radarR = sqrt(radarX * radarX + radarY * radarY + radarZ * radarZ);
    
    size_t i;
    
    for (i=0; i<count; i++) {
        double dx = cos(WF_RADIAN(targets[i].latitude)) * sin(WF_RADIAN(targets[i].longitude)) - radarX;
        double dy = cos(WF_RADIAN(targets[i].latitude)) * cos(WF_RADIAN(targets[i].longitude)) - radarY;
        double dz = sin(WF_RADIAN(targets[i].latitude))                                     - radarZ;
        double dr = sqrt(dx * dx + dy * dy + dz * dz);
        
        /* dot product of the two vectors: 1) earth origin to radar; & 2) earth origin to target */
        double dp = (dx * radarX + dy * radarY + radarZ * dz) / dr / radarR;

        /* raw azimuth */
        double az = WF_DEGREE(atan2((targets[i].longitude - origin->longitude) * cos(WF_RADIAN(origin->latitude)),
                                    (targets[i].latitude - origin->latitude)));
        if (az < 0.0) {
            az += 360.0f;
        }
        
        coords[i].range = WF_EARTH_RADIUS * sqrt(dx * dx + dy * dy + dz * dz);
        coords[i].azimuth = az;
        coords[i].elevation = 90.0f - WF_DEGREE(acos(dp));
    }
}


size_t WFDeriveRadarCellIds(WFRadarCellId *cellIds,              /* An array of cell indices            */
                            const WFSweepStruct *sweep,          /* The radar sweep to map the indices  */
                            const WFRadarCoordStruct *coords,    /* A set of radar cell coordinates     */
                            const WFConfigStruct *config)        /* Number of elements                  */
{
    size_t i, j, k;
    size_t angleIndex, minAngleIndex;
    float halfDeltaAngle = INFINITY;
    float minAngleDelta, angleDelta;
    
    const float invRangeDelta = 1.0f / sweep->rangeDelta;
    
    /* Quick check to ensure the ray spacing is > 0.1 deg in order to avoid processing RHI. */
    for (i=0; i<sweep->rayCount; i++) {
        /* Adjacent ray */
        j = i == 0 ? sweep->rayCount - 1 : i - 1;
        angleDelta = sweep->rays[i].azimuthBegin - sweep->rays[j].azimuthBegin;
        if (angleDelta > 180.0f) {
            angleDelta -= 360.0f;
        } else if (angleDelta < -180.0f) {
            angleDelta += 360.0f;
        }
        if (angleDelta < 0.0f) {
            angleDelta = -angleDelta;
        }
        if (halfDeltaAngle > angleDelta) {
            halfDeltaAngle = angleDelta;
        }
    }
    halfDeltaAngle *= 0.5f;
    if (halfDeltaAngle < 0.1f) {
        fprintf(stderr, "%s : WF : Unexpected ray spacing @ %.2f.\n", now(), halfDeltaAngle);
    }
    
    /* Now, we go through all the wind turbines */
    j = 0;
    angleIndex = 0;
    for (i=0; i<config->count; i++) {
        /* Range index */
        if (coords[i].range < sweep->rangeDelta) {
            printf("Ignoring turbine %zu at range %.2f m\n", j, coords[i].range);
            continue;
        }
        cellIds[j].rangeId = (coords[i].range - sweep->rangeStart) * invRangeDelta;
        if (cellIds[j].rangeId >= sweep->rangeCount) {
            printf("Turbine %zu (%.5f, %.5f) (%.2f, %.1f) is out of range on current sweep.\n",
                   i,
                   config->turbineLocation[i].latitude, config->turbineLocation[i].longitude,
                   coords[i].azimuth, coords[i].range);
            continue;
        }

        /* Azimuth index: Find the best match ray in the sweep */
        minAngleDelta = INFINITY;
        minAngleIndex = 0;
        k = 0;
        while (k < sweep->rayCount) {
            angleDelta = sweep->rays[angleIndex].azimuthBegin - coords[i].azimuth;
            if (angleDelta > 180.0f) {
                angleDelta -= 360.0f;
            } else if (angleDelta < -180.0f) {
                angleDelta += 360.0f;
            }
            if (angleDelta < 0.0f) {
                angleDelta = -angleDelta;
            }
            if (minAngleDelta > angleDelta) {
                minAngleDelta = angleDelta;
                minAngleIndex = angleIndex;
                if (minAngleDelta < halfDeltaAngle) {
                    break;
                }
            }
            angleIndex = angleIndex == sweep->rayCount - 1 ? 0 : angleIndex + 1;
            k++;
        }
        cellIds[j].azimuthId = minAngleIndex;
        
        /* Range extension bin count */
        if (config->multipathLength[i] == 0.0f) {
            cellIds[j].rangeExtensionCount = (size_t)ceilf(config->globalMultipathLength / sweep->rangeDelta);
        } else {
            cellIds[j].rangeExtensionCount = (size_t)ceilf(config->multipathLength[i] / sweep->rangeDelta);
        }

        /* Coordinate is good, go to the next one */
        j++;
    }
    
    return j;
}


size_t WFUniqueRadarCellIds(WFRadarCellId *cellIds,
                            const size_t count)
{
    if (count == 0) {
        return 0;
    }
    
    qsort(cellIds, count, sizeof(WFRadarCellId), WFCompareCellIds);
    
    size_t i;
    size_t newCount = 1;
    WFRadarCellId *newCellIds = (WFRadarCellId *)malloc(count * sizeof(WFRadarCellId));
    
    newCellIds[0] = cellIds[0];
    
    for (i=1; i<count; i++) {
        if (cellIds[i].azimuthId   == cellIds[i-1].azimuthId &&
            cellIds[i].rangeId     == cellIds[i-1].rangeId) {
            /* Do nothing */
        } else {
            newCellIds[newCount++] = cellIds[i];
        }
    }
    memcpy(cellIds, newCellIds, newCount * sizeof(WFRadarCellId));
    free(newCellIds);
    return newCount;
}

size_t WFMultipathExtendRadarCellIds(WFRadarCellId *cellIds,        /* Input/output array of cell indices */
                                     const WFSweepStruct *sweep,    /* Sweep for index derivation         */
                                     const size_t count,            /* Number of cell indices             */
                                     const size_t maxCount)         /* Maximum number of cell indices     */
{
    size_t expandedCount = count;
    size_t i, k;
    char isNew;
    WFRadarCellId testId;
    
    i = 0;
    while (i < count) {
        testId = cellIds[i];
        testId.rangeExtensionCount = 0;
        k = cellIds[i].rangeExtensionCount;
        while (k-- > 0) {
            testId.rangeId++;
            if (testId.rangeId < sweep->rayCount) {
                isNew = WFRadarCellIdIsNewInSet(testId, cellIds, expandedCount);
                if (isNew && expandedCount < maxCount) {
                    cellIds[expandedCount++] = testId;
                }
            }
        }
        i++;
    }
    
    return expandedCount;
}


size_t WFExpandRadarCellIds(WFRadarCellId *cellIds,        /* Input/output array of cell indices    */
                            const WFSweepStruct *sweep,    /* Sweep for index derivation            */
                            const WFExpandFlag flag,       /* Expansion flag                        */
                            const size_t count,            /* Number of cell indices                */
                            const size_t maxCount)         /* Maximum number of cell indices        */
{
    size_t expandedCount = count;
    size_t i;
    char isNew;
    WFRadarCellId testId;

    i = 0;
    while (i < count) {
        if (flag & WF_EXPAND_FLAG_AZIMUTH_NEGATIVE) {
            /* Azimuth - 1 */
            testId = cellIds[i];
            testId.azimuthId = testId.azimuthId == 0 ? sweep->rayCount - 1 : testId.azimuthId - 1;
            isNew = WFRadarCellIdIsNewInSet(testId, cellIds, expandedCount);
            if (isNew && expandedCount < maxCount) {
                cellIds[expandedCount++] = testId;
                if (expandedCount == maxCount) {
                    fprintf(stderr, "%s : WF : WARNING: Reaced array limit during WF_EXPAND_FLAG_AZIMUTH_NEGATIVE.\n", now());
                    break;
                }
            }
        }
        if (flag & WF_EXPAND_FLAG_AZIMUTH_POSITIVE) {
            /* Azimuth + 1 */
            testId = cellIds[i];
            testId.azimuthId = testId.azimuthId == sweep->rayCount - 1 ? 0 : testId.azimuthId + 1;
            isNew = WFRadarCellIdIsNewInSet(testId, cellIds, expandedCount);
            if (isNew && expandedCount < maxCount) {
                cellIds[expandedCount++] = testId;
                if (expandedCount == maxCount) {
                    fprintf(stderr, "%s : WF : Reaced array limit during WF_EXPAND_FLAG_AZIMUTH_POSITIVE.\n", now());
                    break;
                }
            }
        }
        if (flag & WF_EXPAND_FLAG_RANGE_NEGATIVE) {
            /* Range - 1 */
            testId = cellIds[i];
            testId.rangeId--;
            if (testId.rangeId < sweep->rayCount) {
                isNew = WFRadarCellIdIsNewInSet(testId, cellIds, expandedCount);
                if (isNew && expandedCount < maxCount) {
                    cellIds[expandedCount++] = testId;
                    if (expandedCount == maxCount) {
                        fprintf(stderr, "%s : WF : Reaced array limit during WF_EXPAND_FLAG_RANGE_NEGATIVE.\n", now());
                        break;
                    }
                }
            }
        }
        if (flag & WF_EXPAND_FLAG_RANGE_POSITIVE) {
            /* Range + 1 */
            testId = cellIds[i];
            testId.rangeId++;
            if (testId.rangeId < sweep->rayCount) {
                isNew = WFRadarCellIdIsNewInSet(testId, cellIds, expandedCount);
                if (isNew && expandedCount < maxCount) {
                    cellIds[expandedCount++] = testId;
                    if (expandedCount == maxCount) {
                        fprintf(stderr, "%s : WF : Reaced array limit during WF_EXPAND_FLAG_RANGE_POSITIVE.\n", now());
                        break;
                    }
                }
            }
        }
        i++;
    }
    
    if (expandedCount == maxCount) {
        fprintf(stderr, "%s : WF : WARNING: Cell id array not big enough.  %zu / %zu\n", now(), expandedCount, maxCount);
    }
    
    return expandedCount;
}


size_t WFClusterRadarCellIds(WFRadarCellId *cellIds,        /* Input/output array of cell indices    */
                             const WFSweepStruct *sweep,    /* Sweep for index derivation            */
                             const size_t count)            /* Number of cell indices                */
{
    size_t i, k;
    size_t n = 1;
    size_t t;
    WFRadarCellId testId;

    const size_t noName = count + 1;
    
    cellIds[0].clusterId = 0;
    for (i=1; i<count; i++) {
        cellIds[i].clusterId = noName;
    }

    for (i=1; i<count; i++) {
        for (t=0; t<4; t++) {
            testId = cellIds[i];
            switch (t) {
                case 0:
                    /* Azimuth - 1 */
                    testId.azimuthId = testId.azimuthId == 0 ? sweep->rayCount - 1 : testId.azimuthId - 1;
                    break;
                case 1:
                    /* Azimuth + 1 */
                    testId.azimuthId = testId.azimuthId == sweep->rayCount - 1 ? 0 : testId.azimuthId + 1;
                    break;
                case 2:
                    /* Range - 1 */
                    testId.rangeId--;
                    if (testId.rangeId >= sweep->rangeCount) {
                        continue;
                    }
                    break;
                case 3:
                    /* Range + 1 */
                    testId.rangeId++;
                    if (testId.rangeId >= sweep->rangeCount) {
                        continue;
                    }
                    break;
                default:
                    break;
            }
            /* Test if the cell is contiguous with any cells thus far */
            for (k=0; k<i; k++) {
                if (testId.azimuthId == cellIds[k].azimuthId &&
                    testId.rangeId   == cellIds[k].rangeId) {
                    /* cellIds[k] is contiguous with cellIds[i] */
                    if (cellIds[i].clusterId != noName && cellIds[i].clusterId != cellIds[k].clusterId) {
                        size_t minId = MIN(cellIds[i].clusterId, cellIds[k].clusterId);
                        for (size_t x=0; x<i; x++) {
                            if (cellIds[x].clusterId == cellIds[i].clusterId ||
                                cellIds[x].clusterId == cellIds[k].clusterId) {
                                cellIds[x].clusterId = minId;
                            }
                        }
                    }
                    cellIds[i].clusterId = cellIds[k].clusterId;
                }
            }
        }
        if (cellIds[i].clusterId == noName) {
            cellIds[i].clusterId = n++;
        }
    }

    /* If there was no cluster, return early with 0; if there was one, need not consolidate */
    if (n == 0) {
        return 0;
    } else if (n == 1) {
        return 1;
    }

    /* Consolidate the Ids */
    size_t ids[n];
    ids[0] = cellIds[0].clusterId;
    n = 1;
    for (i=1; i<count; i++) {
        k = 0;
        t = 0;
        while (t < n && k == 0) {
            k |= ids[t++] == cellIds[i].clusterId;
        }
        if (k == 0) {
            ids[n++] = cellIds[i].clusterId;
        }
    }
    for (i=0; i<n; i++) {
        for (k=0; k<count; k++) {
            if (cellIds[k].clusterId == ids[i]) {
                cellIds[k].clusterId = i;
            }
        }
    }

    return (n + 1);
}

#pragma mark -
#pragma mark Data Censoring & Reconstruction

void WFCreateCensorRadarCellIds(WFCountStruct *cellCounts,        /* The number of cells                         */
                                WFRadarCellId **ptrCellIds,       /* The pointer to allocate & store the cellIds */
                                const WFSweepStruct *sweep,       /* The radar sweep for cell index derivation   */
                                const WFCoordStruct *origin,      /* Origin of the radar                         */
                                const WFConfigStruct *config)     /* A configuration parameter enclosing unit    */
{
    if (config->count == 0) {
        printf("No cell marked for censoring.\n");
        return;
    }
    
    int i;
    int paddingInRange = (int)ceilf(config->paddingInRangeMeters / sweep->rangeDelta);
    int paddingInAzimuth = (int)ceilf(config->paddingInAzimuthDegrees / sweep->azimuthDelta);
    
    /* Zero out */
    memset(cellCounts, 0, sizeof(WFCountStruct));

    /* Temporary work space to store turbine locations */
    WFRadarCoordStruct *loc = (WFRadarCoordStruct *)malloc(config->count * sizeof(WFRadarCoordStruct));
    
    /* Derive radar-referenced coordinate (elevation, azimuth, range) */
    WFDeriveRadarCoords(loc, config->turbineLocation, origin, config->count);

    /* Pre-allocate the memory space needed */
    WFRadarCellId *cellIds = (WFRadarCellId *)malloc(WF_MAX_CELL_COUNT * sizeof(WFRadarCellId));

    /* Derive radar volume indices */
    size_t count = WFDeriveRadarCellIds(cellIds, sweep, loc, config);
    
    /* No longer need loc, free it. */
    free(loc);

    /* Reduce to a unique set */
    size_t uniqueCount = WFUniqueRadarCellIds(cellIds, count);
    
#ifdef DEBUG
    
    printf("----- User Input Turbine Locations -----\n");
    for (i=0; i<config->count; i++) {
        printf("Target %d  A%-3zu  R%zu\n",
               i, cellIds[i].azimuthId, cellIds[i].rangeId);
    }

    printf("----- Reduced Turbine Locations -----\n");
    for (i=0; i<uniqueCount; i++) {
        printf("Target %d  A%-3zu  R%zu\n",
               i, cellIds[i].azimuthId, cellIds[i].rangeId);
    }

    printf("-------------------------------------\n");
    
#endif
    
    cellCounts->turbines = uniqueCount;
    
    /* Expansion in range due to multipath, this would be the cell count without padding */
    cellCounts->turbinesAndMultipath = WFMultipathExtendRadarCellIds(cellIds, sweep, uniqueCount, WF_MAX_CELL_COUNT);
    
    /* Expansions */
    cellCounts->turbinesMultipathAndPadding = cellCounts->v[1];
    i = 0;
    while (i < MAX(paddingInAzimuth, paddingInRange)) {
        if (i < paddingInAzimuth && i < paddingInRange) {
            cellCounts->turbinesMultipathAndPadding = WFExpandRadarCellIds(cellIds,
                                                                           sweep,
                                                                           WF_EXPAND_FLAG_ALL,
                                                                           cellCounts->turbinesMultipathAndPadding,
                                                                           WF_MAX_CELL_COUNT);
        } else if (i < paddingInRange) {
            cellCounts->turbinesMultipathAndPadding = WFExpandRadarCellIds(cellIds,
                                                                           sweep,
                                                                           WF_EXPAND_FLAG_RANGE_BOTH,
                                                                           cellCounts->turbinesMultipathAndPadding,
                                                                           WF_MAX_CELL_COUNT);
        } else {
            cellCounts->turbinesMultipathAndPadding = WFExpandRadarCellIds(cellIds,
                                                                           sweep,
                                                                           WF_EXPAND_FLAG_AZIMUTH_BOTH,
                                                                           cellCounts->turbinesMultipathAndPadding,
                                                                           WF_MAX_CELL_COUNT);
        }
        i++;
    }

    /* Cluster them into blobs */
    cellCounts->clusters = WFClusterRadarCellIds(cellIds,
                                                 sweep,
                                                 cellCounts->turbinesMultipathAndPadding);
    printf("%s : WF : Found %zu clusters\n", now(), cellCounts->clusters);

    /* Expand more for good cells */
    float aspectRatio = ((float)paddingInAzimuth * WF_RADIAN(sweep->azimuthDelta) * cellIds[cellCounts->badAndGoodCells].rangeId)
                      / ((float)paddingInRange);
    while (aspectRatio > 1.2f) {
        paddingInRange++;
        aspectRatio = ((float)paddingInAzimuth * WF_RADIAN(sweep->azimuthDelta) * cellIds[cellCounts->badAndGoodCells].rangeId)
                    / ((float)paddingInRange);
    }
    printf("%s : WF : Growing (R%d, A%d) for good cells.\n", now(), paddingInRange, paddingInAzimuth);

    cellCounts->badAndGoodCells =cellCounts->turbinesMultipathAndPadding;
    i = 0;
    while (i < MAX(paddingInAzimuth, paddingInRange)) {
        if (i < paddingInAzimuth && i < paddingInRange) {
            cellCounts->badAndGoodCells = WFExpandRadarCellIds(cellIds,
                                                               sweep,
                                                               WF_EXPAND_FLAG_ALL,
                                                               cellCounts->badAndGoodCells,
                                                               WF_MAX_CELL_COUNT);
        } else if (i < paddingInRange) {
            cellCounts->badAndGoodCells = WFExpandRadarCellIds(cellIds,
                                                               sweep,
                                                               WF_EXPAND_FLAG_RANGE_BOTH,
                                                               cellCounts->badAndGoodCells,
                                                               WF_MAX_CELL_COUNT);
        } else {
            cellCounts->badAndGoodCells = WFExpandRadarCellIds(cellIds,
                                                               sweep,
                                                               WF_EXPAND_FLAG_AZIMUTH_BOTH,
                                                               cellCounts->badAndGoodCells,
                                                               WF_MAX_CELL_COUNT);
        }
        i++;
    }

    *ptrCellIds = cellIds;

    if (config->verb) {
        printf("%s : WF : padding %d cells in range & %d cells in azimuth\n", now(), paddingInRange, paddingInAzimuth);
        printf("%s : WF : %zu radar cells\n", now(), uniqueCount);
        printf("%s : WF : %zu expanded radar cells\n", now(), cellCounts->turbinesMultipathAndPadding);
    }
    
    return;
}


void WFReleaseCensorRadarCellIds(WFRadarCellId *cellIds)    /* An array of cell indices to be released */
{
    free(cellIds);
}


void WFBilinearInterpData(WFSweepStruct *sweep,            /* The radar sweep corresponds to the data  */
                          WFCellState **ptrCellStates,     /* An array of housekeeping states          */
                          const WFRadarCellId *cellIds,    /* Cell indices                             */
                          const WFInterpFlag flag,         /* Processing parameters                    */
                          const WFCountStruct *counts)     /* Various counts in cellIds                */
{
    const size_t count = counts->badCells;
    
    WFRadarCellId testId;
    
    WFRadarCellId zeroId = {9999, 0, 0, 0};

    float minValue = INFINITY;
    
    char isInCellIds;
    
    size_t i, n;

    size_t globalId;

    WFCellState *mask = NULL;
    
    if (flag & WF_INTERP_FLAG_GENERATE_CELL_STATE) {
        /* Create a 2D mask and mark all the cells in cellIds */
        size_t memSize = sweep->rangeCount * sweep->rayCount * sizeof(WFCellState);
        mask = (WFCellState *)malloc(memSize);
        if (mask == NULL) {
            fprintf(stderr, "Error allocating memory?\n");
            return;
        }
        memset(mask, 0, memSize);
        for (i=0; i<count; i++) {
            globalId = cellIds[i].azimuthId * sweep->rangeCount + cellIds[i].rangeId;
            mask[globalId] = WF_STATE_REGENERATE;
        }
    } else if (flag & WF_INTERP_FLAG_USE_CELL_STATE) {
        /* Use the supplied cellStates as mask */
        mask = *ptrCellStates;
    } else {
        fprintf(stderr, "flag must be either WF_INTERP_FLAG_GENERATE_CELL_STATE or WF_INTERP_FLAG_USE_CELL_STATE.\n");
        return;
    }

    float westValue;
    float eastValue;
    float southValue;
    float northValue;
    float westValueImag;
    float eastValueImag;
    float southValueImag;
    float northValueImag;
    float northSouthMixedValue;
    float eastWestMixedValue;
    float northSouthMixedValueImag;
    float eastWestMixedValueImag;
    float targetValue;
    float targetValueImag;
    
    /* Go through all the cellIds, find the four good surrounding cells */
    for (i=0; i<count; i++) {
        
        /* Step in Azimuth - 1 */
        n = 0;
        isInCellIds = 1;
        testId = cellIds[i];
        while (isInCellIds && n < sweep->rayCount) {
            testId.azimuthId = testId.azimuthId == 0 ? sweep->rayCount - 1 : testId.azimuthId - 1;
            globalId = testId.azimuthId * sweep->rangeCount + testId.rangeId;
            isInCellIds = mask[globalId] & WF_STATE_REGENERATE;
            n++;
        }
        WFRadarCellId westCellId = isInCellIds ? zeroId : testId;
        
        /* Step in Azimuth + 1 */
        n = 0;
        isInCellIds = 1;
        testId = cellIds[i];
        while (isInCellIds && n < sweep->rayCount) {
            testId.azimuthId = testId.azimuthId == sweep->rayCount - 1 ? 0 : testId.azimuthId + 1;
            globalId = testId.azimuthId * sweep->rangeCount + testId.rangeId;
            isInCellIds = mask[globalId] & WF_STATE_REGENERATE;
        }
        WFRadarCellId eastCellId = isInCellIds ? zeroId : testId;
        
        /* Step in Range - 1 */
        isInCellIds = 1;
        testId = cellIds[i];
        while (isInCellIds && testId.rangeId > 1) {
            testId.rangeId--;
            globalId = testId.azimuthId * sweep->rangeCount + testId.rangeId;
            isInCellIds = mask[globalId] & WF_STATE_REGENERATE;
        }
        WFRadarCellId southCellId = isInCellIds ? zeroId : testId;
        
        /* Step in Range + 1 */
        isInCellIds = 1;
        testId = cellIds[i];
        while (isInCellIds && testId.rangeId < sweep->rangeCount - 1) {
            testId.rangeId++;
            globalId = testId.azimuthId * sweep->rangeCount + testId.rangeId;
            isInCellIds = mask[globalId] & WF_STATE_REGENERATE;
        }
        WFRadarCellId northCellId = isInCellIds ? zeroId : testId;

        /* The four good surrounding cells */
        westValue  = sweep->rays[westCellId.azimuthId].data[westCellId.rangeId];
        eastValue  = sweep->rays[eastCellId.azimuthId].data[eastCellId.rangeId];
        southValue = sweep->rays[southCellId.azimuthId].data[southCellId.rangeId];
        northValue = sweep->rays[northCellId.azimuthId].data[northCellId.rangeId];
        westValueImag  = 0.0f;
        eastValueImag  = 0.0f;
        southValueImag = 0.0f;
        northValueImag = 0.0f;
        
        /* Pre-processing, convert data to linear domain or to complex plane */
        if (flag & WF_INTERP_FLAG_MAP_DB_TO_LINEAR) {
            westValue  = powf(10.0f, 0.1f * westValue);
            eastValue  = powf(10.0f, 0.1f * eastValue);
            southValue = powf(10.0f, 0.1f * southValue);
            northValue = powf(10.0f, 0.1f * northValue);
        } else if (flag & WF_INTERP_FLAG_MAP_TO_COMPLEX) {
            westValue      = westValue / sweep->nyquistVelocity * M_PI;
            westValueImag  = sinf(westValue);
            westValue      = cosf(westValue);
            eastValue      = eastValue / sweep->nyquistVelocity * M_PI;
            eastValueImag  = sinf(eastValue);
            eastValue      = cosf(eastValue);
            northValue     = northValue / sweep->nyquistVelocity * M_PI;
            northValueImag = sinf(northValue);
            northValue     = cosf(northValue);
            southValue     = southValue / sweep->nyquistVelocity * M_PI;
            southValueImag = sinf(southValue);
            southValue     = cosf(southValue);
        }
        

        /* Keep track of the minimum, finite values */
        if (isfinite(westValue)) {
            if (minValue > westValue) {
                minValue = westValue;
            }
        } else {
            westCellId = zeroId;
            westValue = 0.0f;
        }
        if (isfinite(eastValue)) {
            if (minValue > eastValue) {
                minValue = eastValue;
            }
        } else {
            eastCellId = zeroId;
            eastValue = 0.0f;
        }
        if (isfinite(southValue)) {
            if (minValue > southValue) {
                minValue = southValue;
            }
        } else {
            southCellId = zeroId;
            southValue = 0.0f;
        }
        if (isfinite(northValue)) {
            if (minValue > northValue) {
                minValue = northValue;
            }
        } else {
            northCellId = zeroId;
            northValue = 0.0f;
        }
        
        /* Mixing ratio */
        float ratioNorthSouth = (float)(cellIds[i].rangeId - southCellId.rangeId)
                              / (float)(northCellId.rangeId - southCellId.rangeId);
        
        /* Pick the minor sector count from target cell to the west cell */
        int ratioAz = (int)(cellIds[i].azimuthId - westCellId.azimuthId);
        if (ratioAz < 0) {
            ratioAz += sweep->rayCount;
        }
        
        /* Pick the minor sector count from target cell to the west cell */
        int deltaAz = (int)(eastCellId.azimuthId - westCellId.azimuthId);
        if (deltaAz < 0) {
            deltaAz += sweep->rayCount;
        }
        float ratioEastWest = (float)ratioAz
                            / (float)deltaAz;
        if (ratioAz < 0 || deltaAz < 0) {
            fprintf(stderr, "Unexpected mixing ratio. Set to 0.5.\n");
            ratioEastWest = 0.5f;
        }
        
        /* Good cell values */
        northSouthMixedValue     = southValue     + ratioNorthSouth * (northValue     - southValue);
        eastWestMixedValue       = westValue      + ratioEastWest   * (eastValue      - westValue );
        northSouthMixedValueImag = southValueImag + ratioNorthSouth * (northValueImag - southValueImag);
        eastWestMixedValueImag   = westValueImag  + ratioEastWest   * (eastValueImag  - westValueImag );

        targetValue     = 0.5f * (eastWestMixedValue + northSouthMixedValue);
        targetValueImag = 0.5f * (eastWestMixedValueImag + northSouthMixedValueImag);

        /* Post processing, reverse the conversion to linear domain */
        if (flag & WF_INTERP_FLAG_MAP_DB_TO_LINEAR) {
            targetValue = 10.0f * log10f(targetValue);
        } else if (flag & WF_INTERP_FLAG_MAP_TO_COMPLEX) {
            targetValue = atan2f(targetValueImag, targetValue) / M_PI * sweep->nyquistVelocity;
        }
        
        /* Assign to the data cell */
        sweep->rays[cellIds[i].azimuthId].data[cellIds[i].rangeId] = targetValue;
    }
    
    /* Preserve censoring from reflectivity product */
    if (flag & WF_INTERP_FLAG_GENERATE_CELL_STATE) {
        for (i=0; i<count; i++) {
            globalId = cellIds[i].azimuthId * sweep->rangeCount + cellIds[i].rangeId;
            float *ptrData = &sweep->rays[cellIds[i].azimuthId].data[cellIds[i].rangeId];
            if (*ptrData < minValue || !isfinite(*ptrData)) {
                *ptrData = NAN;
                mask[globalId] |= WF_STATE_CENSOR;
            }
        }
    } else if (flag & WF_INTERP_FLAG_USE_CELL_STATE) {
        for (i=0; i<count; i++) {
            globalId = cellIds[i].azimuthId * sweep->rangeCount + cellIds[i].rangeId;
            if (mask[globalId] & WF_STATE_CENSOR) {
                sweep->rays[cellIds[i].azimuthId].data[cellIds[i].rangeId] = NAN;
            }
        }
    }

    if (ptrCellStates == NULL) {
        free(mask);
    } else {
        *ptrCellStates = mask;
    }
}


void WFSelectiveBilinearInterpData(WFSweepStruct *sweep,            /* The radar sweep corresponds to the data  */
                                   WFCellState **ptrCellStates,     /* An array of housekeeping states          */
                                   const WFRadarCellId *cellIds,    /* Cell indices                             */
                                   const WFInterpFlag flag,         /* Processing parameters                    */
                                   const WFCountStruct *counts)     /* Various counts in cellIds                */
{
    const size_t count = counts->badCells;
    
    WFRadarCellId testId;
    
    WFRadarCellId zeroId = {9999, 0, 0, 0};
        
    float minValue = INFINITY;
    
    char isInCellIds;
    
    size_t i, n;
    
    size_t globalId;
    
    float *ptrData;
    
    WFCellState *mask = NULL;
       
    if (flag & WF_INTERP_FLAG_GENERATE_CELL_STATE) {
        /* Create a 2D mask and mark all the cells in cellIds */
        size_t mem_size = sweep->rangeCount * sweep->rayCount * sizeof(WFCellState);
        mask = (WFCellState *)malloc(mem_size);
        if (mask == NULL) {
            fprintf(stderr, "Error allocating memory?\n");
            return;
        }
        memset(mask, 0, mem_size);
        for (i=0; i<count; i++) {
            globalId = cellIds[i].azimuthId * sweep->rangeCount + cellIds[i].rangeId;
            mask[globalId] = WF_STATE_REGENERATE;
        }
    } else if (flag & WF_INTERP_FLAG_USE_CELL_STATE) {
        /* Use the supplied cell_states as mask */
        mask = *ptrCellStates;
    } else {
        fprintf(stderr, "flag must be either WF_INTERP_FLAG_GENERATE_CELL_STATE or WF_INTERP_FLAG_USE_CELL_STATE.\n");
        return;
    }
    
    if (flag & WF_INTERP_FLAG_MAP_TO_COMPLEX && sweep->nyquistVelocity == 0.0f) {
        fprintf(stderr, "Nyquist velocity has not been set.\n");
        return;
    }
    
    float westValue;
    float eastValue;
    float southValue;
    float northValue;
    float westValueImag;
    float eastValueImag;
    float southValueImag;
    float northValueImag;
    float northSouthMixedValue;
    float eastWestMixedValue;
    float northSouthMixedValueImag;
    float eastWestMixedValueImag;
    float targetValue;
    float targetValueImag;

    /* Go through all the cellIds, find the four good surrounding cells */
    for (i=0; i<count; i++) {
        
        /* Step in Azimuth - 1 */
        n = 0;
        isInCellIds = 1;
        testId = cellIds[i];
        while (isInCellIds && n < sweep->rayCount) {
            testId.azimuthId = testId.azimuthId == 0 ? sweep->rayCount - 1 : testId.azimuthId - 1;
            globalId = testId.azimuthId * sweep->rangeCount + testId.rangeId;
            isInCellIds = mask[globalId] & WF_STATE_REGENERATE;
            n++;
        }
        WFRadarCellId westCellId = isInCellIds ? zeroId : testId;
        
        /* Step in Azimuth + 1 */
        n = 0;
        isInCellIds = 1;
        testId = cellIds[i];
        while (isInCellIds && n < sweep->rayCount) {
            testId.azimuthId = testId.azimuthId == sweep->rayCount - 1 ? 0 : testId.azimuthId + 1;
            globalId = testId.azimuthId * sweep->rangeCount + testId.rangeId;
            isInCellIds = mask[globalId] & WF_STATE_REGENERATE;
        }
        WFRadarCellId eastCellId = isInCellIds ? zeroId : testId;
        
        /* Step in Range - 1 */
        isInCellIds = 1;
        testId = cellIds[i];
        while (isInCellIds && testId.rangeId > 1) {
            testId.rangeId--;
            globalId = testId.azimuthId * sweep->rangeCount + testId.rangeId;
            isInCellIds = mask[globalId] & WF_STATE_REGENERATE;
        }
        WFRadarCellId southCellId = isInCellIds ? zeroId : testId;
        
        /* Step in Range + 1 */
        isInCellIds = 1;
        testId = cellIds[i];
        while (isInCellIds && testId.rangeId < sweep->rangeCount - 1) {
            testId.rangeId++;
            globalId = testId.azimuthId * sweep->rangeCount + testId.rangeId;
            isInCellIds = mask[globalId] & WF_STATE_REGENERATE;
        }
        WFRadarCellId northCellId = isInCellIds ? zeroId : testId;
        
        /* The four good surrounding cells */
        westValue  = sweep->rays[westCellId.azimuthId].data[westCellId.rangeId];
        eastValue  = sweep->rays[eastCellId.azimuthId].data[eastCellId.rangeId];
        southValue = sweep->rays[southCellId.azimuthId].data[southCellId.rangeId];
        northValue = sweep->rays[northCellId.azimuthId].data[northCellId.rangeId];
        westValueImag  = 0.0f;
        eastValueImag  = 0.0f;
        southValueImag = 0.0f;
        northValueImag = 0.0f;
        
        /* Keep track of the minimum, finite values */
        if (isfinite(westValue)) {
            if (minValue > westValue) {
                minValue = westValue;
            }
        } else {
            westCellId = zeroId;
        }
        if (isfinite(eastValue)) {
            if (minValue > eastValue) {
                minValue = eastValue;
            }
        } else {
            eastCellId = zeroId;
        }
        if (isfinite(southValue)) {
            if (minValue > southValue) {
                minValue = southValue;
            }
        } else {
            southCellId = zeroId;
        }
        if (isfinite(northValue)) {
            if (minValue > northValue) {
                minValue = northValue;
            }
        } else {
            northCellId = zeroId;
        }
        
        /* Pre-processing, convert data to linear domain or to complex plane */
        if (flag & WF_INTERP_FLAG_MAP_DB_TO_LINEAR) {
            westValue  = powf(10.0f, 0.1f * westValue);
            eastValue  = powf(10.0f, 0.1f * eastValue);
            southValue = powf(10.0f, 0.1f * southValue);
            northValue = powf(10.0f, 0.1f * northValue);
        } else if (flag & WF_INTERP_FLAG_MAP_TO_COMPLEX) {
            westValue      = westValue / sweep->nyquistVelocity * M_PI;
            westValueImag  = sinf(westValue);
            westValue      = cosf(westValue);
            eastValue      = eastValue / sweep->nyquistVelocity * M_PI;
            eastValueImag  = sinf(eastValue);
            eastValue      = cosf(eastValue);
            northValue     = northValue / sweep->nyquistVelocity * M_PI;
            northValueImag = sinf(northValue);
            northValue     = cosf(northValue);
            southValue     = southValue / sweep->nyquistVelocity * M_PI;
            southValueImag = sinf(southValue);
            southValue     = cosf(southValue);
        }
        
        /* Mixing ratio */
        float ratioNorthSouth = (float)(cellIds[i].rangeId - southCellId.rangeId)
                              / (float)(northCellId.rangeId - southCellId.rangeId);
        /* Pick the minor sector count from target cell to the west cell */
        int ratioAz = (int)(cellIds[i].azimuthId - westCellId.azimuthId);
        if (ratioAz < 0) {
            ratioAz += sweep->rayCount;
        }
        /* Pick the minor sector count from target cell to the west cell */
        int deltaAz = (int)(eastCellId.azimuthId - westCellId.azimuthId);
        if (deltaAz < 0) {
            deltaAz += sweep->rayCount;
        }
        float ratioEastWest = (float)ratioAz
                            / (float)deltaAz;
        if (ratioAz < 0 || deltaAz < 0) {
            fprintf(stderr, "Unexpected mixing ratio. Set to 0.5.\n");
            ratioEastWest = 0.5f;
        }
        
#ifdef DEBUG_HEAVY
        
        printf("westCell @ (%zu, %zu, %zu) = %.2f, %.2f\n",
               westCellId.azimuthId,
               westCellId.rangeId,
               westCellId.clusterId,
               westValue, westValueImag);
        printf("eastCell @ (%zu, %zu, %zu) = %.2f, %.2f\n",
               eastCellId.azimuthId,
               eastCellId.rangeId,
               eastCellId.clusterId,
               eastValue, eastValueImag);
        printf("southCell @ (%zu, %zu, %zu) = %.2f, %.2f\n",
               southCellId.azimuthId,
               southCellId.rangeId,
               southCellId.clusterId,
               southValue, southValueImag);
        printf("northCell @ (%zu, %zu, %zu) = %.2f, %.2f\n",
               northCellId.azimuthId,
               northCellId.rangeId,
               northCellId.clusterId,
               northValue, northValueImag);
        printf("mixing ratio  %.2f  %.2f\n", ratioEastWest, ratioNorthSouth);
        
#endif
        
        northSouthMixedValue     = (southValue     + ratioNorthSouth * (northValue     - southValue));
        eastWestMixedValue       = (westValue      + ratioEastWest   * (eastValue      - westValue ));
        northSouthMixedValueImag = (southValueImag + ratioNorthSouth * (northValueImag - southValueImag));
        eastWestMixedValueImag   = (westValueImag  + ratioEastWest   * (eastValueImag  - westValueImag ));

        /* Decision tree for various reconstruction option based on the availability of surrounding cells */
        if (westCellId.clusterId == zeroId.clusterId) {
            if (eastCellId.clusterId == zeroId.clusterId) {
                if (southCellId.clusterId == zeroId.clusterId) {
                    if (northCellId.clusterId == zeroId.clusterId) {
                        /* Nothing */
                        targetValue = NAN;
                        targetValueImag = NAN;
                    } else {
                        /* Only north */
                        targetValue = NAN;
                        targetValueImag = NAN;
                    }
                } else {
                    if (northCellId.clusterId == zeroId.clusterId) {
                        /* Only south */
                        targetValue = NAN;
                        targetValueImag = NAN;
                    } else {
                        /* Only south, north */
                        targetValue     = 0.5f * northSouthMixedValue;
                        targetValueImag = 0.5f * northSouthMixedValueImag;
#ifdef DEBUG
                        printf("-*- data = %.2f / %.2f\n", targetValue, minValue);
#endif
                    }
                }
            } else {
                if (southCellId.clusterId == zeroId.clusterId) {
                    if (northCellId.clusterId == zeroId.clusterId) {
                        /* Only east */
                        targetValue = NAN;
                        targetValueImag = NAN;
                    } else {
                        /* Only east, north */
                        targetValue = NAN;
                        targetValueImag = NAN;
                    }
                } else {
                    if (northCellId.clusterId == zeroId.clusterId) {
                        /* Only east, south */
                        targetValue = NAN;
                        targetValueImag = NAN;
                    } else {
                        /* Only east, south, north */
                        targetValue     = ratioEastWest * northSouthMixedValue;
                        targetValueImag = ratioEastWest * northSouthMixedValueImag;
#ifdef DEBUG
                        printf("-** data = %.2f / %.2f\n", targetValue, minValue);
#endif
                    }
                }
            }
        } else {
            if (eastCellId.clusterId == zeroId.clusterId) {
                if (southCellId.clusterId == zeroId.clusterId) {
                    if (northCellId.clusterId == zeroId.clusterId) {
                        /* Only west */
                        targetValue = NAN;
                        targetValueImag = NAN;
                    } else {
                        /* Only west, north */
                        targetValue = NAN;
                        targetValueImag = NAN;
                    }
                } else {
                    if (northCellId.clusterId == zeroId.clusterId) {
                        /* Only west, south */
                        targetValue = NAN;
                        targetValueImag = NAN;
                    } else {
                        /* Only west, south, north */
                        targetValue     = (1.0f - ratioEastWest) * northSouthMixedValue;
                        targetValueImag = (1.0f - ratioEastWest) * northSouthMixedValueImag;
#ifdef DEBUG
                        printf("**- data = %.2f <-- %.2f + %.2f\n", targetValue, westValue, northSouthMixedValue);
#endif
                    }
                }
            } else {
                if (southCellId.clusterId == zeroId.clusterId) {
                    if (northCellId.clusterId == zeroId.clusterId) {
                        /* Only west, east */
                        targetValue     = 0.5f * eastWestMixedValue;
                        targetValueImag = 0.5f * eastWestMixedValueImag;
                    } else {
                        /* Only west, east, north */
                        targetValue     = ratioNorthSouth * eastWestMixedValue;
                        targetValueImag = ratioNorthSouth * eastWestMixedValueImag;
                    }
                } else {
                    if (northCellId.clusterId == zeroId.clusterId) {
                        /* Only west, east, south */
                        targetValue     = (1.0f - ratioNorthSouth) * eastWestMixedValue;
                        targetValueImag = (1.0f - ratioNorthSouth) * eastWestMixedValueImag;
                    } else {
                        /* All cells are okay */
                        targetValue     = 0.5f * (eastWestMixedValue     + northSouthMixedValue    );
                        targetValueImag = 0.5f * (eastWestMixedValueImag + northSouthMixedValueImag);
#ifdef DEBUG
                        printf("*** data = %.2f / %.2f\n", targetValue, minValue);
#endif
                    }
                }
            }
        }
        
        /* Post processing, reverse the conversion to linear domain */
        if (flag & WF_INTERP_FLAG_MAP_DB_TO_LINEAR) {
            targetValue = 10.0f * log10f(targetValue);
        } else if (flag & WF_INTERP_FLAG_MAP_TO_COMPLEX) {
            targetValue = atan2f(targetValueImag, targetValue) / M_PI * sweep->nyquistVelocity;
        }
        
        /* Assign to the data cell */
        sweep->rays[cellIds[i].azimuthId].data[cellIds[i].rangeId] = targetValue;
    }

    /* Preserve censoring from reflectivity product */
    if (flag & WF_INTERP_FLAG_GENERATE_CELL_STATE) {
        for (i=0; i<count; i++) {
            globalId = cellIds[i].azimuthId * sweep->rangeCount + cellIds[i].rangeId;
            ptrData = &sweep->rays[cellIds[i].azimuthId].data[cellIds[i].rangeId];
            if (*ptrData < minValue || !isfinite(*ptrData)) {
                *ptrData = NAN;
                mask[globalId] |= WF_STATE_CENSOR;
            }
        }
    } else if (flag & WF_INTERP_FLAG_USE_CELL_STATE) {
        for (i=0; i<count; i++) {
            globalId = cellIds[i].azimuthId * sweep->rangeCount + cellIds[i].rangeId;
            ptrData = &sweep->rays[cellIds[i].azimuthId].data[cellIds[i].rangeId];
            if (mask[globalId] & WF_STATE_CENSOR) {
                *ptrData = NAN;
            }
        }
    }
    
    /* Pass the mask out or free it */
    if (ptrCellStates == NULL) {
        free(mask);
    } else {
        *ptrCellStates = mask;
    }
}


void WFInverseDistanceInterpData(WFSweepStruct *sweep,            /* The radar sweep corresponds to the data  */
                                 WFCellState **ptrCellStates,     /* An array of housekeeping states          */
                                 const WFRadarCellId *cellIds,    /* Cell indices                             */
                                 const WFInterpFlag flag,         /* Processing parameters                    */
                                 const WFCountStruct *counts)     /* Various counts in cellIds                */
{
    WFCellState *mask = NULL;
    
    float minValue = INFINITY;
    float *ptrData;
        
    size_t i, j;
    size_t globalId;
    
    if (flag & WF_INTERP_FLAG_GENERATE_CELL_STATE) {
        /* Create a 2D mask and mark all the cells in cellIds */
        size_t mem_size = sweep->rangeCount * sweep->rayCount * sizeof(WFCellState);
        mask = (WFCellState *)malloc(mem_size);
        if (mask == NULL) {
            fprintf(stderr, "Error allocating memory?\n");
            return;
        }
        memset(mask, 0, mem_size);
        for (i=0; i<counts->badCells; i++) {
            globalId = cellIds[i].azimuthId * sweep->rangeCount + cellIds[i].rangeId;
            mask[globalId] = WF_STATE_REGENERATE;
        }
    } else if (flag & WF_INTERP_FLAG_USE_CELL_STATE) {
        /* Use the supplied cell_states as mask */
        mask = *ptrCellStates;
    } else {
        fprintf(stderr, "flag must be either WF_INTERP_FLAG_GENERATE_CELL_STATE or WF_INTERP_FLAG_USE_CELL_STATE.\n");
        return;
    }
    
    if (flag & WF_INTERP_FLAG_MAP_TO_COMPLEX && sweep->nyquistVelocity == 0.0f) {
        fprintf(stderr, "Nyquist velocity has not been set.\n");
        return;
    }

    float w = 0.0f;
    float wSum = 0.0f;
    float *valuesReal = (float *)malloc(counts->badAndGoodCells * sizeof(float));
    float *valuesImag = (float *)malloc(counts->badAndGoodCells * sizeof(float));
    memset(valuesReal, 0, counts->badCells * sizeof(float));
    memset(valuesImag, 0, counts->badCells * sizeof(float));
    
    if (flag & WF_INTERP_FLAG_MAP_DB_TO_LINEAR) {
        for (i=counts->badCells; i<counts->badAndGoodCells; i++) {
            valuesReal[i] = powf(10.0f, 0.1f * sweep->rays[cellIds[i].azimuthId].data[cellIds[i].rangeId]);
            if (isfinite(sweep->rays[cellIds[i].azimuthId].data[cellIds[i].rangeId]) &&
                minValue > sweep->rays[cellIds[i].azimuthId].data[cellIds[i].rangeId]) {
                minValue = sweep->rays[cellIds[i].azimuthId].data[cellIds[i].rangeId];
            }
        } /* for */
    } else if (flag & WF_INTERP_FLAG_MAP_TO_COMPLEX) {
        for (i=counts->badCells; i<counts->badAndGoodCells; i++) {
            valuesReal[i] = cosf(sweep->rays[cellIds[i].azimuthId].data[cellIds[i].rangeId] / sweep->nyquistVelocity * M_PI);
            valuesImag[i] = sinf(sweep->rays[cellIds[i].azimuthId].data[cellIds[i].rangeId] / sweep->nyquistVelocity * M_PI);
        } /* for */
    } else {
        for (i=counts->badCells; i<counts->badAndGoodCells; i++) {
            valuesReal[i] = sweep->rays[cellIds[i].azimuthId].data[cellIds[i].rangeId];
            if (isfinite(sweep->rays[cellIds[i].azimuthId].data[cellIds[i].rangeId]) &&
                minValue > valuesReal[i]) {
                minValue = valuesReal[i];
            }
        } /* for */
    }
    
    for (i=0; i<counts->badCells; i++) {
        wSum = 0.0f;
        /* Go around the cluster boundary */
        for (j=counts->badCells; j<counts->badAndGoodCells; j++) {
            if (cellIds[j].clusterId == cellIds[i].clusterId) {
                /* More accurate to be described as id distance but sufficiently close for this application */
                w = (cellIds[j].azimuthId - cellIds[i].azimuthId) * (cellIds[j].azimuthId - cellIds[i].azimuthId)
                  + (cellIds[j].rangeId   - cellIds[i].rangeId)   * (cellIds[j].rangeId   - cellIds[i].rangeId);
                w = powf(1.0f / w, 2.0f);
                wSum += w;
                if (isfinite(valuesReal[j])) {
                    valuesReal[i] += w * valuesReal[j];
                    valuesImag[i] += w * valuesImag[j];
                }
            }
        } /* for */
        valuesReal[i] /= wSum;
        valuesImag[i] /= wSum;
    }
    
    if (flag & WF_INTERP_FLAG_MAP_DB_TO_LINEAR) {
        for (i=0; i<counts->badCells; i++) {
            sweep->rays[cellIds[i].azimuthId].data[cellIds[i].rangeId] = 10.0f * log10f(valuesReal[i]);
            
#ifdef DEBUG_HEAVY
            
            printf("i = %zu   v = %.2f dB / %.2f\n",
                   i,
                   sweep->rays[cellIds[i].azimuthId].data[cellIds[i].rangeId],
                   valuesReal[i]);
            
#endif

        } /* for */
    } else if (flag & WF_INTERP_FLAG_MAP_TO_COMPLEX) {
        for (i=0; i<counts->badCells; i++) {
            sweep->rays[cellIds[i].azimuthId].data[cellIds[i].rangeId] = atan2f(valuesImag[i], valuesReal[i]) / M_PI * sweep->nyquistVelocity;
        } /* for */
    } else {
        for (i=0; i<counts->badCells; i++) {
            sweep->rays[cellIds[i].azimuthId].data[cellIds[i].rangeId] = valuesReal[i];
        } /* for */
    }

    free(valuesReal);
    free(valuesImag);
    
    /* Preserve censoring from reflectivity product */
    if (flag & WF_INTERP_FLAG_GENERATE_CELL_STATE) {
        for (i=0; i<counts->badCells; i++) {
            globalId = cellIds[i].azimuthId * sweep->rangeCount + cellIds[i].rangeId;
            ptrData = &sweep->rays[cellIds[i].azimuthId].data[cellIds[i].rangeId];
            if (*ptrData < minValue || !isfinite(*ptrData)) {
                *ptrData = NAN;
                mask[globalId] |= WF_STATE_CENSOR;
            }
        } /* for */
    } else if (flag & WF_INTERP_FLAG_USE_CELL_STATE) {
        for (i=0; i<counts->badCells; i++) {
            globalId = cellIds[i].azimuthId * sweep->rangeCount + cellIds[i].rangeId;
            ptrData = &sweep->rays[cellIds[i].azimuthId].data[cellIds[i].rangeId];
            if (mask[globalId] & WF_STATE_CENSOR) {
                *ptrData = NAN;
            }
        } /* for */
    }
    
    /* Pass the mask out or free it */
    if (ptrCellStates == NULL) {
        free(mask);
    } else {
        *ptrCellStates = mask;
    }
}

