# Copyright 2014 The Chromium Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.
import logging

from telemetry.core import exceptions
from telemetry.page import page as page_module
from telemetry.page import page_set as page_set_module


class SafebrowsingPage(page_module.Page):

  """
  Why: Expect 'malware ahead' page. Use a short navigation timeout because no
  response will be received.
  """

  def __init__(self, url, page_set, expect_timeout):
    super(SafebrowsingPage, self).__init__(url=url, page_set=page_set)
    self.archive_data_file = '../data/chrome_proxy_safebrowsing.json'
    self._expect_timeout = expect_timeout

  def RunNavigateSteps(self, action_runner):
    try:
      action_runner.Navigate(self.url, timeout_in_seconds=5)
    except exceptions.TimeoutException as e:
      if self._expect_timeout:
        logging.warning('Navigation timeout on page %s', self.url)
      else:
        raise e


class SafebrowsingPageSet(page_set_module.PageSet):

  """ Chrome proxy test sites """

  def __init__(self, expect_timeout=False):
    super(SafebrowsingPageSet, self).__init__(
      archive_data_file='../data/chrome_proxy_safebrowsing.json')

    self.AddUserStory(
        SafebrowsingPage('http://www.ianfette.org/', self, expect_timeout))
