/*
 *  Copyright (c) 2014 Dmitry Kazakov <dimula73@gmail.com>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#include "kis_fill_interval_map.h"
#include "kis_fill_interval_map_p.h"

#include "kis_fill_sanity_checks.h"


KisFillIntervalMap::KisFillIntervalMap()
    : m_d(new Private)
{
}

KisFillIntervalMap::~KisFillIntervalMap()
{
}

void KisFillIntervalMap::insertInterval(const KisFillInterval &interval)
{
    Private::GlobalMap::iterator rowMap = m_d->map.find(interval.row);
    if (rowMap == m_d->map.end()) {
        rowMap = m_d->map.insert(interval.row, Private::LineIntervalMap());
    }

    rowMap->insert(interval.start, interval);
}

void KisFillIntervalMap::cropInterval(KisFillInterval *interval)
{
    Private::IteratorRange range;
    range = m_d->findFirstIntersectingInterval(*interval);

    Private::LineIntervalMap::iterator it = range.beginIt;

    while (interval->isValid() && it != range.endIt) {
        if (it->start < interval->start && it->end >= interval->start) {
            int savedIntervalStart = interval->start;
            interval->start = it->end + 1;

            // it might happen that we need to split a backward interval
            // into two pieces
            if (it->end > interval->end) {
                KisFillInterval newInterval(interval->end + 1, it->end, it->row);
                range.rowMapIt->insert(newInterval.start, newInterval);
            }

            it->end = savedIntervalStart - 1;
        } else if (it->start <= interval->end && it->end > interval->end) {
            int savedIntervalEnd = interval->end;
            interval->end = it->start - 1;
            it->start = savedIntervalEnd + 1;
        } else if (it->start > interval->end) {
            break;
        }
#ifdef ENABLE_FILL_SANITY_CHECKS
        else if (it->start >= interval->start && it->end <= interval->end) {
            SANITY_ASSERT_MSG(0, "FATAL: The backward interval cannot fully reside inside the forward interval");
            it->invalidate();
            interval->invalidate();
        }

        SANITY_ASSERT_MSG(it->isValid(), "FATAL: The backward interval cannot become invalid during the crop action");

#endif /* ENABLE_FILL_SANITY_CHECKS */

        it++;
    }
}

KisFillIntervalMap::Private::IteratorRange
KisFillIntervalMap::Private::findFirstIntersectingInterval(const KisFillInterval &interval)
{
    Private::GlobalMap::iterator rowMap = map.find(interval.row);
    if (rowMap == map.end()) {
        return IteratorRange();
    }

    LineIntervalMap::iterator it = rowMap->begin();
    LineIntervalMap::iterator end = rowMap->end();

    while(it != end) {
        if (it->end < interval.start) {
            ++it;
        } else if (it->start > interval.end) {
            it = end;
            break;
        } else {
            break;
        }
    }

    return IteratorRange(it, end, rowMap);
}

QStack<KisFillInterval> KisFillIntervalMap::fetchAllIntervals(int rowCorrection) const
{
    QStack<KisFillInterval> intervals;

    Private::GlobalMap::const_iterator rowMapIt = m_d->map.constBegin();
    Private::GlobalMap::const_iterator rowMapEndIt = m_d->map.constEnd();

    while (rowMapIt != rowMapEndIt) {
        Private::LineIntervalMap::const_iterator it = rowMapIt->constBegin();
        Private::LineIntervalMap::const_iterator end = rowMapIt->constEnd();

        while(it != end) {
            KisFillInterval interval = *it;
            interval.row += rowCorrection;
            intervals.append(interval);
            ++it;
        }

        ++rowMapIt;
    }

    return intervals;
}

void KisFillIntervalMap::clear()
{
    m_d->map.clear();
}