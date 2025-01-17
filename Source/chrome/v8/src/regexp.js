// Copyright 2012 the V8 project authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

var $regexpExec;
var $regexpExecNoTests;
var $regexpLastMatchInfo;
var $regexpLastMatchInfoOverride;
var harmony_regexps = false;
var harmony_unicode_regexps = false;

(function(global, utils) {

%CheckIsBootstrapping();

// -------------------------------------------------------------------
// Imports

var GlobalRegExp = global.RegExp;
var InternalPackedArray = utils.InternalPackedArray;

// -------------------------------------------------------------------

// Property of the builtins object for recording the result of the last
// regexp match.  The property $regexpLastMatchInfo includes the matchIndices
// array of the last successful regexp match (an array of start/end index
// pairs for the match and all the captured substrings), the invariant is
// that there are at least two capture indeces.  The array also contains
// the subject string for the last successful match.
$regexpLastMatchInfo = new InternalPackedArray(
 2,                 // REGEXP_NUMBER_OF_CAPTURES
 "",                // Last subject.
 UNDEFINED,         // Last input - settable with RegExpSetInput.
 0,                 // REGEXP_FIRST_CAPTURE + 0
 0                  // REGEXP_FIRST_CAPTURE + 1
);

// Override last match info with an array of actual substrings.
// Used internally by replace regexp with function.
// The array has the format of an "apply" argument for a replacement
// function.
$regexpLastMatchInfoOverride = null;

// -------------------------------------------------------------------

// A recursive descent parser for Patterns according to the grammar of
// ECMA-262 15.10.1, with deviations noted below.
function DoConstructRegExp(object, pattern, flags) {
  // RegExp : Called as constructor; see ECMA-262, section 15.10.4.
  if (IS_REGEXP(pattern)) {
    if (!IS_UNDEFINED(flags)) throw MakeTypeError(kRegExpFlags);
    flags = (pattern.global ? 'g' : '')
        + (pattern.ignoreCase ? 'i' : '')
        + (pattern.multiline ? 'm' : '');
    if (harmony_unicode_regexps)
        flags += (pattern.unicode ? 'u' : '');
    if (harmony_regexps)
        flags += (pattern.sticky ? 'y' : '');
    pattern = pattern.source;
  }

  pattern = IS_UNDEFINED(pattern) ? '' : $toString(pattern);
  flags = IS_UNDEFINED(flags) ? '' : $toString(flags);

  %RegExpInitializeAndCompile(object, pattern, flags);
}


function RegExpConstructor(pattern, flags) {
  if (%_IsConstructCall()) {
    DoConstructRegExp(this, pattern, flags);
  } else {
    // RegExp : Called as function; see ECMA-262, section 15.10.3.1.
    if (IS_REGEXP(pattern) && IS_UNDEFINED(flags)) {
      return pattern;
    }
    return new GlobalRegExp(pattern, flags);
  }
}

// Deprecated RegExp.prototype.compile method.  We behave like the constructor
// were called again.  In SpiderMonkey, this method returns the regexp object.
// In JSC, it returns undefined.  For compatibility with JSC, we match their
// behavior.
function RegExpCompileJS(pattern, flags) {
  // Both JSC and SpiderMonkey treat a missing pattern argument as the
  // empty subject string, and an actual undefined value passed as the
  // pattern as the string 'undefined'.  Note that JSC is inconsistent
  // here, treating undefined values differently in
  // RegExp.prototype.compile and in the constructor, where they are
  // the empty string.  For compatibility with JSC, we match their
  // behavior.
  if (this == GlobalRegExp.prototype) {
    // We don't allow recompiling RegExp.prototype.
    throw MakeTypeError(kIncompatibleMethodReceiver,
                        'RegExp.prototype.compile', this);
  }
  if (IS_UNDEFINED(pattern) && %_ArgumentsLength() != 0) {
    DoConstructRegExp(this, 'undefined', flags);
  } else {
    DoConstructRegExp(this, pattern, flags);
  }
}


function DoRegExpExec(regexp, string, index) {
  var result = %_RegExpExec(regexp, string, index, $regexpLastMatchInfo);
  if (result !== null) $regexpLastMatchInfoOverride = null;
  return result;
}


// This is kind of performance sensitive, so we want to avoid unnecessary
// type checks on inputs. But we also don't want to inline it several times
// manually, so we use a macro :-)
macro RETURN_NEW_RESULT_FROM_MATCH_INFO(MATCHINFO, STRING)
  var numResults = NUMBER_OF_CAPTURES(MATCHINFO) >> 1;
  var start = MATCHINFO[CAPTURE0];
  var end = MATCHINFO[CAPTURE1];
  // Calculate the substring of the first match before creating the result array
  // to avoid an unnecessary write barrier storing the first result.
  var first = %_SubString(STRING, start, end);
  var result = %_RegExpConstructResult(numResults, start, STRING);
  result[0] = first;
  if (numResults == 1) return result;
  var j = REGEXP_FIRST_CAPTURE + 2;
  for (var i = 1; i < numResults; i++) {
    start = MATCHINFO[j++];
    if (start != -1) {
      end = MATCHINFO[j];
      result[i] = %_SubString(STRING, start, end);
    }
    j++;
  }
  return result;
endmacro


function RegExpExecNoTests(regexp, string, start) {
  // Must be called with RegExp, string and positive integer as arguments.
  var matchInfo = %_RegExpExec(regexp, string, start, $regexpLastMatchInfo);
  if (matchInfo !== null) {
    $regexpLastMatchInfoOverride = null;
    RETURN_NEW_RESULT_FROM_MATCH_INFO(matchInfo, string);
  }
  regexp.lastIndex = 0;
  return null;
}


function RegExpExecJS(string) {
  if (!IS_REGEXP(this)) {
    throw MakeTypeError(kIncompatibleMethodReceiver,
                        'RegExp.prototype.exec', this);
  }

  string = TO_STRING_INLINE(string);
  var lastIndex = this.lastIndex;

  // Conversion is required by the ES5 specification (RegExp.prototype.exec
  // algorithm, step 5) even if the value is discarded for non-global RegExps.
  var i = TO_INTEGER(lastIndex);

  var updateLastIndex = this.global || (harmony_regexps && this.sticky);
  if (updateLastIndex) {
    if (i < 0 || i > string.length) {
      this.lastIndex = 0;
      return null;
    }
  } else {
    i = 0;
  }

  // matchIndices is either null or the $regexpLastMatchInfo array.
  var matchIndices = %_RegExpExec(this, string, i, $regexpLastMatchInfo);

  if (IS_NULL(matchIndices)) {
    this.lastIndex = 0;
    return null;
  }

  // Successful match.
  $regexpLastMatchInfoOverride = null;
  if (updateLastIndex) {
    this.lastIndex = $regexpLastMatchInfo[CAPTURE1];
  }
  RETURN_NEW_RESULT_FROM_MATCH_INFO(matchIndices, string);
}


// One-element cache for the simplified test regexp.
var regexp_key;
var regexp_val;

// Section 15.10.6.3 doesn't actually make sense, but the intention seems to be
// that test is defined in terms of String.prototype.exec. However, it probably
// means the original value of String.prototype.exec, which is what everybody
// else implements.
function RegExpTest(string) {
  if (!IS_REGEXP(this)) {
    throw MakeTypeError(kIncompatibleMethodReceiver,
                        'RegExp.prototype.test', this);
  }
  string = TO_STRING_INLINE(string);

  var lastIndex = this.lastIndex;

  // Conversion is required by the ES5 specification (RegExp.prototype.exec
  // algorithm, step 5) even if the value is discarded for non-global RegExps.
  var i = TO_INTEGER(lastIndex);

  if (this.global || (harmony_regexps && this.sticky)) {
    if (i < 0 || i > string.length) {
      this.lastIndex = 0;
      return false;
    }
    // matchIndices is either null or the $regexpLastMatchInfo array.
    var matchIndices = %_RegExpExec(this, string, i, $regexpLastMatchInfo);
    if (IS_NULL(matchIndices)) {
      this.lastIndex = 0;
      return false;
    }
    $regexpLastMatchInfoOverride = null;
    this.lastIndex = $regexpLastMatchInfo[CAPTURE1];
    return true;
  } else {
    // Non-global, non-sticky regexp.
    // Remove irrelevant preceeding '.*' in a test regexp.  The expression
    // checks whether this.source starts with '.*' and that the third char is
    // not a '?'.  But see https://code.google.com/p/v8/issues/detail?id=3560
    var regexp = this;
    if (regexp.source.length >= 3 &&
        %_StringCharCodeAt(regexp.source, 0) == 46 &&  // '.'
        %_StringCharCodeAt(regexp.source, 1) == 42 &&  // '*'
        %_StringCharCodeAt(regexp.source, 2) != 63) {  // '?'
      regexp = TrimRegExp(regexp);
    }
    // matchIndices is either null or the $regexpLastMatchInfo array.
    var matchIndices = %_RegExpExec(regexp, string, 0, $regexpLastMatchInfo);
    if (IS_NULL(matchIndices)) {
      this.lastIndex = 0;
      return false;
    }
    $regexpLastMatchInfoOverride = null;
    return true;
  }
}

function TrimRegExp(regexp) {
  if (!%_ObjectEquals(regexp_key, regexp)) {
    regexp_key = regexp;
    regexp_val =
      new GlobalRegExp(%_SubString(regexp.source, 2, regexp.source.length),
                       (regexp.ignoreCase ? regexp.multiline ? "im" : "i"
                                          : regexp.multiline ? "m" : ""));
  }
  return regexp_val;
}


function RegExpToString() {
  if (!IS_REGEXP(this)) {
    throw MakeTypeError(kIncompatibleMethodReceiver,
                        'RegExp.prototype.toString', this);
  }
  var result = '/' + this.source + '/';
  if (this.global) result += 'g';
  if (this.ignoreCase) result += 'i';
  if (this.multiline) result += 'm';
  if (harmony_unicode_regexps && this.unicode) result += 'u';
  if (harmony_regexps && this.sticky) result += 'y';
  return result;
}


// Getters for the static properties lastMatch, lastParen, leftContext, and
// rightContext of the RegExp constructor.  The properties are computed based
// on the captures array of the last successful match and the subject string
// of the last successful match.
function RegExpGetLastMatch() {
  if ($regexpLastMatchInfoOverride !== null) {
    return OVERRIDE_MATCH($regexpLastMatchInfoOverride);
  }
  var regExpSubject = LAST_SUBJECT($regexpLastMatchInfo);
  return %_SubString(regExpSubject,
                     $regexpLastMatchInfo[CAPTURE0],
                     $regexpLastMatchInfo[CAPTURE1]);
}


function RegExpGetLastParen() {
  if ($regexpLastMatchInfoOverride) {
    var override = $regexpLastMatchInfoOverride;
    if (override.length <= 3) return '';
    return override[override.length - 3];
  }
  var length = NUMBER_OF_CAPTURES($regexpLastMatchInfo);
  if (length <= 2) return '';  // There were no captures.
  // We match the SpiderMonkey behavior: return the substring defined by the
  // last pair (after the first pair) of elements of the capture array even if
  // it is empty.
  var regExpSubject = LAST_SUBJECT($regexpLastMatchInfo);
  var start = $regexpLastMatchInfo[CAPTURE(length - 2)];
  var end = $regexpLastMatchInfo[CAPTURE(length - 1)];
  if (start != -1 && end != -1) {
    return %_SubString(regExpSubject, start, end);
  }
  return "";
}


function RegExpGetLeftContext() {
  var start_index;
  var subject;
  if (!$regexpLastMatchInfoOverride) {
    start_index = $regexpLastMatchInfo[CAPTURE0];
    subject = LAST_SUBJECT($regexpLastMatchInfo);
  } else {
    var override = $regexpLastMatchInfoOverride;
    start_index = OVERRIDE_POS(override);
    subject = OVERRIDE_SUBJECT(override);
  }
  return %_SubString(subject, 0, start_index);
}


function RegExpGetRightContext() {
  var start_index;
  var subject;
  if (!$regexpLastMatchInfoOverride) {
    start_index = $regexpLastMatchInfo[CAPTURE1];
    subject = LAST_SUBJECT($regexpLastMatchInfo);
  } else {
    var override = $regexpLastMatchInfoOverride;
    subject = OVERRIDE_SUBJECT(override);
    var match = OVERRIDE_MATCH(override);
    start_index = OVERRIDE_POS(override) + match.length;
  }
  return %_SubString(subject, start_index, subject.length);
}


// The properties $1..$9 are the first nine capturing substrings of the last
// successful match, or ''.  The function RegExpMakeCaptureGetter will be
// called with indices from 1 to 9.
function RegExpMakeCaptureGetter(n) {
  return function foo() {
    if ($regexpLastMatchInfoOverride) {
      if (n < $regexpLastMatchInfoOverride.length - 2) {
        return OVERRIDE_CAPTURE($regexpLastMatchInfoOverride, n);
      }
      return '';
    }
    var index = n * 2;
    if (index >= NUMBER_OF_CAPTURES($regexpLastMatchInfo)) return '';
    var matchStart = $regexpLastMatchInfo[CAPTURE(index)];
    var matchEnd = $regexpLastMatchInfo[CAPTURE(index + 1)];
    if (matchStart == -1 || matchEnd == -1) return '';
    return %_SubString(LAST_SUBJECT($regexpLastMatchInfo), matchStart, matchEnd);
  };
}

// -------------------------------------------------------------------

%FunctionSetInstanceClassName(GlobalRegExp, 'RegExp');
%AddNamedProperty(
    GlobalRegExp.prototype, 'constructor', GlobalRegExp, DONT_ENUM);
%SetCode(GlobalRegExp, RegExpConstructor);

$installFunctions(GlobalRegExp.prototype, DONT_ENUM, [
  "exec", RegExpExecJS,
  "test", RegExpTest,
  "toString", RegExpToString,
  "compile", RegExpCompileJS
]);

// The length of compile is 1 in SpiderMonkey.
%FunctionSetLength(GlobalRegExp.prototype.compile, 1);

// The properties `input` and `$_` are aliases for each other.  When this
// value is set the value it is set to is coerced to a string.
// Getter and setter for the input.
var RegExpGetInput = function() {
  var regExpInput = LAST_INPUT($regexpLastMatchInfo);
  return IS_UNDEFINED(regExpInput) ? "" : regExpInput;
};
var RegExpSetInput = function(string) {
  LAST_INPUT($regexpLastMatchInfo) = $toString(string);
};

%OptimizeObjectForAddingMultipleProperties(GlobalRegExp, 22);
%DefineAccessorPropertyUnchecked(GlobalRegExp, 'input', RegExpGetInput,
                                 RegExpSetInput, DONT_DELETE);
%DefineAccessorPropertyUnchecked(GlobalRegExp, '$_', RegExpGetInput,
                                 RegExpSetInput, DONT_ENUM | DONT_DELETE);

// The properties multiline and $* are aliases for each other.  When this
// value is set in SpiderMonkey, the value it is set to is coerced to a
// boolean.  We mimic that behavior with a slight difference: in SpiderMonkey
// the value of the expression 'RegExp.multiline = null' (for instance) is the
// boolean false (i.e., the value after coercion), while in V8 it is the value
// null (i.e., the value before coercion).

// Getter and setter for multiline.
var multiline = false;
var RegExpGetMultiline = function() { return multiline; };
var RegExpSetMultiline = function(flag) { multiline = flag ? true : false; };

%DefineAccessorPropertyUnchecked(GlobalRegExp, 'multiline', RegExpGetMultiline,
                                 RegExpSetMultiline, DONT_DELETE);
%DefineAccessorPropertyUnchecked(GlobalRegExp, '$*', RegExpGetMultiline,
                                 RegExpSetMultiline,
                                 DONT_ENUM | DONT_DELETE);


var NoOpSetter = function(ignored) {};


// Static properties set by a successful match.
%DefineAccessorPropertyUnchecked(GlobalRegExp, 'lastMatch', RegExpGetLastMatch,
                                 NoOpSetter, DONT_DELETE);
%DefineAccessorPropertyUnchecked(GlobalRegExp, '$&', RegExpGetLastMatch,
                                 NoOpSetter, DONT_ENUM | DONT_DELETE);
%DefineAccessorPropertyUnchecked(GlobalRegExp, 'lastParen', RegExpGetLastParen,
                                 NoOpSetter, DONT_DELETE);
%DefineAccessorPropertyUnchecked(GlobalRegExp, '$+', RegExpGetLastParen,
                                 NoOpSetter, DONT_ENUM | DONT_DELETE);
%DefineAccessorPropertyUnchecked(GlobalRegExp, 'leftContext',
                                 RegExpGetLeftContext, NoOpSetter,
                                 DONT_DELETE);
%DefineAccessorPropertyUnchecked(GlobalRegExp, '$`', RegExpGetLeftContext,
                                 NoOpSetter, DONT_ENUM | DONT_DELETE);
%DefineAccessorPropertyUnchecked(GlobalRegExp, 'rightContext',
                                 RegExpGetRightContext, NoOpSetter,
                                 DONT_DELETE);
%DefineAccessorPropertyUnchecked(GlobalRegExp, "$'", RegExpGetRightContext,
                                 NoOpSetter, DONT_ENUM | DONT_DELETE);

for (var i = 1; i < 10; ++i) {
  %DefineAccessorPropertyUnchecked(GlobalRegExp, '$' + i,
                                   RegExpMakeCaptureGetter(i), NoOpSetter,
                                   DONT_DELETE);
}
%ToFastProperties(GlobalRegExp);

$regexpExecNoTests = RegExpExecNoTests;
$regexpExec = DoRegExpExec;

})
