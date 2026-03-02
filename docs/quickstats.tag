<?xml version='1.0' encoding='UTF-8' standalone='yes' ?>
<tagfile doxygen_version="1.12.0">
  <compound kind="file">
    <name>quickstats.hpp</name>
    <path>quickstats/</path>
    <filename>quickstats_8hpp.html</filename>
    <namespace>quickstats</namespace>
  </compound>
  <compound kind="class">
    <name>quickstats::SingleQuantileFixedNumber</name>
    <filename>classquickstats_1_1SingleQuantileFixedNumber.html</filename>
    <templarg>typename Output_</templarg>
    <templarg>typename Number_</templarg>
    <member kind="function">
      <type></type>
      <name>SingleQuantileFixedNumber</name>
      <anchorfile>classquickstats_1_1SingleQuantileFixedNumber.html</anchorfile>
      <anchor>a618b718231dcec8c9d7aa91018a2449c</anchor>
      <arglist>(const Number_ num, const Output_ quantile)</arglist>
    </member>
    <member kind="function">
      <type>Output_</type>
      <name>operator()</name>
      <anchorfile>classquickstats_1_1SingleQuantileFixedNumber.html</anchorfile>
      <anchor>aeca16d5d48b528ec4131038ef3099875</anchor>
      <arglist>(Input_ *ptr) const</arglist>
    </member>
    <member kind="function">
      <type>Output_</type>
      <name>operator()</name>
      <anchorfile>classquickstats_1_1SingleQuantileFixedNumber.html</anchorfile>
      <anchor>aed209822c087f3471c66dbc5bb3ec5ca</anchor>
      <arglist>(const Number_ nnz, Input_ *ptr) const</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>quickstats::SingleQuantileVariableNumber</name>
    <filename>classquickstats_1_1SingleQuantileVariableNumber.html</filename>
    <templarg>typename Output_</templarg>
    <templarg>typename Number_</templarg>
    <member kind="function">
      <type></type>
      <name>SingleQuantileVariableNumber</name>
      <anchorfile>classquickstats_1_1SingleQuantileVariableNumber.html</anchorfile>
      <anchor>a74f9c583d75973e4dd9ebd7df39c32a6</anchor>
      <arglist>(Number_ max_num, const double quantile)</arglist>
    </member>
    <member kind="function">
      <type>Output_</type>
      <name>operator()</name>
      <anchorfile>classquickstats_1_1SingleQuantileVariableNumber.html</anchorfile>
      <anchor>af44e3ce8db366b828cae2b07e6bee510</anchor>
      <arglist>(const Number_ num, Input_ *ptr)</arglist>
    </member>
    <member kind="function">
      <type>Output_</type>
      <name>operator()</name>
      <anchorfile>classquickstats_1_1SingleQuantileVariableNumber.html</anchorfile>
      <anchor>a13af43abca3637a7d9f80b624322bc9b</anchor>
      <arglist>(const Number_ num, const Number_ nnz, Input_ *ptr)</arglist>
    </member>
  </compound>
  <compound kind="namespace">
    <name>quickstats</name>
    <filename>namespacequickstats.html</filename>
    <class kind="class">quickstats::SingleQuantileFixedNumber</class>
    <class kind="class">quickstats::SingleQuantileVariableNumber</class>
    <member kind="function">
      <type>Output_</type>
      <name>median</name>
      <anchorfile>namespacequickstats.html</anchorfile>
      <anchor>a739d1e42f6a0d27ad2e70b3464e54f81</anchor>
      <arglist>(const Number_ num, Input_ *const ptr)</arglist>
    </member>
    <member kind="function">
      <type>Output_</type>
      <name>median</name>
      <anchorfile>namespacequickstats.html</anchorfile>
      <anchor>a31b88fd01011c1ea144c2cabe83e496b</anchor>
      <arglist>(const Number_ num, const Number_ nnz, Input_ *const value)</arglist>
    </member>
  </compound>
  <compound kind="page">
    <name>index</name>
    <title>Quickly compute simple statistics</title>
    <filename>index.html</filename>
    <docanchor file="index.html" title="Quickly compute simple statistics">md__2github_2workspace_2README</docanchor>
  </compound>
</tagfile>
