/*
 * Copyright (C) 1999 Lars Knoll (knoll@kde.org)
 * Copyright (C) 2004, 2005, 2006, 2007, 2008, 2009 Apple Inc. All rights reserved.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include "config.h"
#include "core/layout/PendingSelection.h"

#include "core/dom/Document.h"
#include "core/editing/FrameSelection.h"
#include "core/editing/VisiblePosition.h"
#include "core/editing/VisibleUnits.h"
#include "core/html/HTMLTextFormControlElement.h"

namespace blink {

PendingSelection::PendingSelection()
    : m_hasPendingSelection(false)
    , m_shouldShowBlockCursor(false)
{
    clear();
}

void PendingSelection::setSelection(const FrameSelection& selection)
{
    m_selection = selection.selection();
    m_shouldShowBlockCursor = selection.shouldShowBlockCursor();
    m_hasPendingSelection = true;
}

void PendingSelection::clear()
{
    m_hasPendingSelection = false;
    m_selection = VisibleSelection();
    m_shouldShowBlockCursor = false;
}

bool PendingSelection::isInDocument(const Document& document) const
{
    Position start = m_selection.start();
    if (start.isNotNull() && (!start.inDocument() || start.document() != document))
        return false;
    Position end = m_selection.end();
    if (end.isNotNull() && (!end.inDocument() || end.document() != document))
        return false;
    Position extent = m_selection.extent();
    if (extent.isNotNull() && (!extent.inDocument() || extent.document() != document))
        return false;
    return true;
}

VisibleSelection PendingSelection::calcVisibleSelection() const
{
    Position start = m_selection.start();
    Position end = m_selection.end();
    SelectionType selectionType = VisibleSelection::selectionType(start, end);
    EAffinity affinity = m_selection.affinity();

    bool paintBlockCursor = m_shouldShowBlockCursor && selectionType == SelectionType::CaretSelection && !isLogicalEndOfLine(VisiblePosition(end, affinity));
    VisibleSelection selection;
    if (enclosingTextFormControl(start)) {
        Position endPosition = paintBlockCursor ? m_selection.extent().next() : end;
        selection.setWithoutValidation(start, endPosition);
        return selection;
    }

    VisiblePosition visibleStart = VisiblePosition(start, selectionType == SelectionType::RangeSelection ? DOWNSTREAM : affinity);
    if (paintBlockCursor) {
        VisiblePosition visibleExtent(end, affinity);
        visibleExtent = visibleExtent.next(CanSkipOverEditingBoundary);
        return VisibleSelection(visibleStart, visibleExtent);
    }
    VisiblePosition visibleEnd(end, selectionType == SelectionType::RangeSelection ? UPSTREAM : affinity);
    return VisibleSelection(visibleStart, visibleEnd);
}

DEFINE_TRACE(PendingSelection)
{
    visitor->trace(m_selection);
}

} // namespace blink
