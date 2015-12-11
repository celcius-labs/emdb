# Querying

## Data Storage

In order to query data, it must be in a consumable form.  The form that has
been chosen for this is JSON.

## Query Structure

Queries depend on a structured tree defined and iterated through.  Currently,
this must be generated, but plans are to include the ability to have a parser
and to have a more structured query language.

```
typedef struct Where {
  WhereType type;
  unsigned char *key;
  unsigned char *value;
  ValueType value_type;
  unsigned char child_count;
  void **children;
} Where;
```

A `WhereType` defines the query action to take.  It is an `enum` that is defined
as follows:

```
typedef enum WhereType {
  and     = 0,
  or      = 1,
  equals  = 2,
  gt      = 3,
  lt      = 4,
  gte     = 5,
  lte     = 6,
  between = 7,
  in      = 8
} WhereType;
```

C is a typed language, whereas JSON is pretty free flow.  A comparison needs
to figure out what type of comparison to do - `numeric` vs `string`.  That's
where `ValueType` comes in.

```
typedef enum ValueType {
  string,
  integer,
  floatingpoint
} ValueType;
```

### Basic Queries

Basic queries are singular operators that are fairly quick to execute, such
as checking for equality.

To write a simple query, to return all records that "type" is set to
"temperature", the query would look like:

```
Query q = {
  WhereType.equals,
  (unsigned char *) "type",
  (unsigned char *) "temperature",
  ValueType.string,
  0,
  NULL
};
```

In this case, the `WhereType` is set to `equals`, which will attempt to do
typed comparisons against a `string` because `value_type` is set to `string`.
