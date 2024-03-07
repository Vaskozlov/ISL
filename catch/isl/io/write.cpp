#include <isl/detail/debug/debug.hpp>
import isl;

TEST_CASE("ReadWriteToFile", "[IO]")
{
    static constexpr std::string_view content = R"(
BasedOnStyle: LLVM
AccessModifierOffset: -4
AlignAfterOpenBracket: AlwaysBreak
AlignConsecutiveAssignments: None
AlignConsecutiveDeclarations: None
AlignEscapedNewlines: Right
AlignOperands: Align
AlignTrailingComments: true
AllowAllParametersOfDeclarationOnNextLine: false
AllowShortBlocksOnASingleLine: Empty
AllowShortCaseLabelsOnASingleLine: false
AllowShortFunctionsOnASingleLine: None
AllowShortIfStatementsOnASingleLine: false
AllowShortLoopsOnASingleLine: false
AllowShortLambdasOnASingleLine: None
AlwaysBreakAfterDefinitionReturnType: None
AlwaysBreakAfterReturnType: None
AlwaysBreakBeforeMultilineStrings: true
AlwaysBreakTemplateDeclarations: Yes
BinPackArguments: true
BinPackParameters: true
BraceWrapping:
  AfterClass: true
  AfterEnum: true
  AfterFunction: true
  AfterNamespace: true
  AfterObjCDeclaration: false
  AfterStruct: true
  AfterUnion: false
  BeforeCatch: false
  BeforeElse: false
  IndentBraces: false
  SplitEmptyFunction: false
  SplitEmptyNamespace: true
  SplitEmptyRecord: true
BreakAfterJavaFieldAnnotations: true
BreakBeforeBinaryOperators: None
BreakBeforeBraces: Custom
BreakBeforeInheritanceComma: true
BreakBeforeTernaryOperators: true
BreakConstructorInitializers: BeforeComma
BreakConstructorInitializersBeforeComma: false
BreakStringLiterals: true
ColumnLimit: 100
CommentPragmas: '^ IWYU pragma:'
CompactNamespaces: false
ConstructorInitializerAllOnOneLineOrOnePerLine: false
ConstructorInitializerIndentWidth: 2
ContinuationIndentWidth: 4
Cpp11BracedListStyle: true
DerivePointerAlignment: false
DisableFormat: false
ExperimentalAutoDetectBinPacking: true
FixNamespaceComments: true
IncludeCategories:
  - Priority: 2
    Regex: ^"(llvm|llvm-c|clang|clang-c)/
  - Priority: 3
    Regex: ^(<|"(gtest|gmock|isl|json)/)
  - Priority: 1
    Regex: .*
IncludeIsMainRegex: (Test)?$
IndentCaseLabels: false
IndentWidth: 4
IndentWrappedFunctionNames: true
JavaScriptQuotes: Leave
JavaScriptWrapImports: true
KeepEmptyLinesAtTheStartOfBlocks: false
Language: Cpp
MacroBlockBegin: ''
MacroBlockEnd: ''
MaxEmptyLinesToKeep: 2
NamespaceIndentation: All
ObjCBlockIndentWidth: 7
ObjCSpaceAfterProperty: true
ObjCSpaceBeforeProtocolList: false
PointerAlignment: Right
ReflowComments: true
SortIncludes: CaseInsensitive
SortUsingDeclarations: true
SpaceAfterCStyleCast: false
SpaceAfterTemplateKeyword: false
SpaceBeforeAssignmentOperators: true
SpaceBeforeParens: ControlStatements
SpaceInEmptyParentheses: false
SpacesBeforeTrailingComments: 0
SpacesInAngles: false
SpacesInCStyleCastParentheses: false
SpacesInContainerLiterals: true
SpacesInParentheses: false
SpacesInSquareBrackets: false
Standard: Latest
TabWidth: 4
UseTab: Never
IndentPPDirectives: AfterHash
)";

    auto tmp_path = std::filesystem::temp_directory_path();
    auto tmp_filename = std::tmpnam(nullptr);
    tmp_path.append(tmp_filename);

    isl::io::writeToFile(tmp_path, content, isl::string_view{"some extension"});
    REQUIRE(isl::io::readFile(tmp_path) == (std::string{content} + "some extension"));
}