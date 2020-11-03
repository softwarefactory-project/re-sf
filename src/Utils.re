open Belt;

let makeError = (message, value) => Decco.{path: "", message, value};

// Lift an Option.t('a) to a Deccod.result('a)
let note =
    (v: option('a), error: Decco.decodeError)
    : Result.t('a, Decco.decodeError) =>
  switch (v) {
  | Some(v) => v->Ok
  | None => error->Error
  };

// Sequence a list of Optional (https://hackage.haskell.org/package/base-4.14.0.0/docs/Control-Monad.html#v:sequence)
let rec sequence = (xs: list(option('a))): option(list('a)) =>
  switch (xs) {
  | [] => Some([])
  | [Some(x), ...rest] =>
    sequence(rest)->Option.map(rest => List.add(rest, x))
  | [None, ..._rest] => None
  };

// list(f(a))
// Sequence a list of Decco.result
let rec sequenceR = (xs: list(Decco.result('a))): Decco.result(list('a)) =>
  switch (xs) {
  | [] => []->Ok
  | [Ok(v), ...rest] =>
    sequenceR(rest)->Result.map(rest => List.add(rest, v))
  | [Error(e), ..._rest] => e->Error
  };

// Transform { k: v } to (k, v) when there is only one k
let decodeSingleKeyDict =
    (dict: Js.Dict.t(Js.Json.t)): option((string, Js.Json.t)) =>
  switch (Js.Dict.entries(dict)) {
  | [|(name, v)|] => (name, v)->Some
  | _ => None
  };

// Add a key/value to a json dictionary
let addJsonKey = (key: string, value: Js.Json.t, dict: Js.Dict.t(Js.Json.t)) =>
  Js.Dict.fromArray(
    Js.Array.concat(Js.Dict.entries(dict), [|(key, value)|]),
  );

// This function transform dictionary of the form:
//   { name: { k: v }, other: { x: y } }
// to:
//   [{ name: "name", k: v }, { name: "other", x: y }]
let decodeDictName = (jsonObject: Js.Json.t): option(list(Js.Json.t)) =>
  jsonObject
  ->Js.Json.decodeObject
  ->Option.flatMap(dict =>
      dict
      ->Js.Dict.entries
      ->List.fromArray
      ->List.map(((name, v)) =>
          Js.Json.decodeObject(v)
          ->Option.flatMap(attrs =>
              Js.Json.object_(
                addJsonKey("name", name->Js.Json.string, attrs),
              )
              ->Some
            )
        )
      ->sequence
    );

let dictFilter = (dict, f) =>
  dict->Js.Dict.entries->Belt.Array.keep(f)->Js.Dict.fromArray;
let dictKeyFilter = (dict, f) => dict->dictFilter(((k, _v)) => f(k));
let dictValueFilter = (dict, f) => dict->dictFilter(((_k, v)) => f(v));
let isNull = v =>
  switch (v->Js.Json.classify) {
  | JSONNull => true
  | _ => false
  };
let dictGetString = (dict, k) =>
  dict
  ->Js.Dict.get(k)
  ->Option.flatMap(name_value => name_value->Js.Json.decodeString);

// This function transform dictionary of the form:
//   { name: "name", k: v}
// to:
//   { "name": { k: v } }
let encodeDictName = (jsonObject: Js.Json.t): option(Js.Json.t) =>
  jsonObject
  ->Js.Json.decodeObject
  ->Option.flatMap(dict =>
      dict
      ->dictGetString("name")
      ->Option.flatMap(name =>
          Js.Json.object_(
            Js.Dict.fromList([
              (
                name,
                Js.Json.object_(
                  dict
                  ->dictKeyFilter(k => k != "name")
                  ->dictValueFilter(v => !isNull(v)),
                ),
              ),
            ]),
          )
          ->Some
        )
    );

let decodeOneDictName = (jsonObject: Js.Json.t): option(Js.Json.t) =>
  switch (decodeDictName(jsonObject)) {
  | Some([v]) => Some(v)
  | _ => None
  };
