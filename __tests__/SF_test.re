open Jest;
open Tenant;

describe("Test resources JSON parsing", () => {
  open Expect;

  let example_tenant = {|
  {
    "default-connection": "rdoproject.org",
    "description": "The rdoproject.org tenant",
    "tenant-options": {
        "zuul/report-build-page": true,
        "zuul/web-root": "https://review.rdoproject.org/zuul/"
    },
    "url": "https://review.rdoproject.org/manage",
    "name": "rdoproject.org"
  }
  |};
  let expected_tenant = {
    name: "rdoproject.org",
    default_connection: Some("rdoproject.org"),
    description: Some("The rdoproject.org tenant"),
    url: "https://review.rdoproject.org/manage",
  };

  test("parse tenant object", () => {
    let parsed_tenant = Tenant.parse(Json.parseOrRaise(example_tenant));
    expect(parsed_tenant.name) |> toBe(expected_tenant.name);
  });
});
