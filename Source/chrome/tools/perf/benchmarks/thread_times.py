# Copyright 2014 The Chromium Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

from core import perf_benchmark

from benchmarks import silk_flags
from measurements import thread_times
from telemetry import benchmark
import page_sets

class _ThreadTimes(perf_benchmark.PerfBenchmark):
  @classmethod
  def AddBenchmarkCommandLineArgs(cls, parser):
    parser.add_option('--report-silk-details', action='store_true',
                      help='Report details relevant to silk.')

  @classmethod
  def Name(cls):
    return 'thread_times'

  @classmethod
  def ValueCanBeAddedPredicate(cls, value, _):
    # Default to only reporting per-frame metrics.
    return 'per_second' not in value.name

  def CreatePageTest(self, options):
    return thread_times.ThreadTimes(options.report_silk_details)


@benchmark.Enabled('android')
class ThreadTimesKeySilkCases(_ThreadTimes):
  """Measures timeline metrics while performing smoothness action on key silk
  cases."""
  page_set = page_sets.KeySilkCasesPageSet

  @classmethod
  def Name(cls):
    return 'thread_times.key_silk_cases'

@benchmark.Enabled('android', 'linux')
class ThreadTimesKeyHitTestCases(_ThreadTimes):
  """Measure timeline metrics while performing smoothness action on key hit
  testing cases."""
  page_set = page_sets.KeyHitTestCasesPageSet

  @classmethod
  def Name(cls):
    return 'thread_times.key_hit_test_cases'

@benchmark.Enabled('android')
class ThreadTimesFastPathMobileSites(_ThreadTimes):
  """Measures timeline metrics while performing smoothness action on
  key mobile sites labeled with fast-path tag.
  http://www.chromium.org/developers/design-documents/rendering-benchmarks"""
  page_set = page_sets.KeyMobileSitesSmoothPageSet
  options = {'story_label_filter' : 'fastpath'}

  @classmethod
  def Name(cls):
    return 'thread_times.key_mobile_sites_smooth'


@benchmark.Enabled('android')
class ThreadTimesSimpleMobileSites(_ThreadTimes):
  """Measures timeline metric using smoothness action on simple mobile sites
  http://www.chromium.org/developers/design-documents/rendering-benchmarks"""
  page_set = page_sets.SimpleMobileSitesPageSet

  @classmethod
  def Name(cls):
    return 'thread_times.simple_mobile_sites'


@benchmark.Disabled('win') # crbug.com/443781
class ThreadTimesCompositorCases(_ThreadTimes):
  """Measures timeline metrics while performing smoothness action on
  tough compositor cases, using software rasterization.

  http://www.chromium.org/developers/design-documents/rendering-benchmarks"""
  page_set = page_sets.ToughCompositorCasesPageSet
  def SetExtraBrowserOptions(self, options):
    silk_flags.CustomizeBrowserOptionsForSoftwareRasterization(options)

  @classmethod
  def Name(cls):
    return 'thread_times.tough_compositor_cases'

@benchmark.Enabled('android')
class ThreadTimesPolymer(_ThreadTimes):
  """Measures timeline metrics while performing smoothness action on
  Polymer cases."""
  page_set = page_sets.PolymerPageSet
  @classmethod
  def Name(cls):
    return 'thread_times.polymer'

@benchmark.Enabled('android')
class ThreadTimesKeyIdlePowerCases(_ThreadTimes):
  """Measures timeline metrics for sites that should be idle in foreground
  and background scenarios. The metrics are per-second rather than per-frame."""
  page_set = page_sets.KeyIdlePowerCasesPageSet

  @classmethod
  def Name(cls):
    return 'thread_times.key_idle_power_cases'

  @classmethod
  def ValueCanBeAddedPredicate(cls, value, _):
    # Only report per-second metrics.
    return 'per_frame' not in value.name and 'mean_frame' not in value.name
