/*  -*- c++ -*-
 * Copyright 2026 by Tatu Lahtela <lahtela@iki.fi>
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef SWITCHER_H
#define SWITCHER_H

#include <KRunner/AbstractRunner>

class Switcher : public KRunner::AbstractRunner
{
    Q_OBJECT

public:
    Switcher(QObject *parent, const KPluginMetaData &metaData, const QVariantList &args);

    ~Switcher() override;

    void match(KRunner::RunnerContext &context) override;

    void run(const KRunner::RunnerContext &context, const KRunner::QueryMatch &match) override;
};

#endif
