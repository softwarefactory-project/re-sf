open Jest;
open Expect;

let read = fn => Node.Fs.readFileSync("tests/fixtures/" ++ fn, `utf8);
let parse = (json, decoder) => {
  let v = decoder(Js.Json.parseExn(json));
  Belt.Result.isError(v) ? Js.log(v) : ();
  v |> Belt.Result.getExn;
};

describe("Test resources JSON parsing", () => {
  test("parse tenant object", () => {
    let json = {|
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
    let expected =
      Tenant.{
        name: "rdoproject.org",
        default_connection: Some("rdoproject.org"),
        description: Some("The rdoproject.org tenant"),
        url: "https://review.rdoproject.org/manage",
      };
    let tenant = json->parse(Tenant.decode);
    expect(tenant.name) |> toBe(expected.name);
  });

  test("parse connection object", () => {
    let json = {|
        {
          "base-url": "https://pagure.io",
          "type": "pagure",
          "name": "pagure.io"
        }
    |};
    let expected =
      Connection.{
        base_url: Some("https://pagure.io"),
        connection_type: Pagure,
        name: "pagure.io",
      };
    let connection = json->parse(Connection.decode);
    expect(connection.name) |> toBe(expected.name);
  });

  let projectJson = {|
       {
         "contacts": [
           "harrymichal@seznam.cz"
         ],
         "description": "Unprivileged development environment",
         "tenant": "local",
         "website": "https://github.com/debarshiray/toolbox",
         "name": "toolbox",
         "source-repositories": [
           {
             "containers/toolbox": {
               "connection": "github.com",
               "zuul/include": [],
               "zuul/exclude-unprotected-branches": true
             }
           },
           "software-factory/cauth",
           {
             "software-factory/managesf": {}
           }
         ]
       }
     |};

  test("parse project object", () => {
    let expected =
      Project.{
        name: "toolbox",
        contacts: Some(["harrymichal@seznam.cz"]),
        description: "Unprivileged development environment",
        tenant: Some("local"),
        website: Some("https://github.com/debarshiray/toolbox"),
        issue_tracker_url: None,
        review_dashboard: None,
        mailing_lists: Some([]),
        connection: None,
        documentation: None,
        options: Some([]),
        sourceRepositories: [
          SourceRepository.Full({
            name: "containers/toolbox",
            connection: Some("github.com"),
            zuul_include: Some([Job]),
          }),
          SourceRepository.Name("software-factory/cauth"),
          SourceRepository.Full({
            name: "software-factory/managesf",
            connection: None,
            zuul_include: None,
          }),
        ],
      };
    let parsed = projectJson->parse(Project.decode);
    expect(parsed.name) |> toBe(expected.name);
  });

  test("parse repo object", () => {
    let parsed = "repo.json"->read->parse(Repo.decode);
    expect(parsed.name) |> toBe("test/project-distgit");
  });

  test("filter project by tenant", () => {
    let parsed = projectJson->parse(Project.decode);
    expect((
      [parsed] |> Project.filterProjectsByTenant("local") |> Belt.List.length,
      [parsed]
      |> Project.filterProjectsByTenant("alternate")
      |> Belt.List.length,
    ))
    |> toEqual((1, 0));
  });

  let raw = "resources.json"->read->Js.String.trim;
  let parsed = raw->parse(Resources.decode);
  test("parse resources object", () => {
    let isToolbox = (project: Project.t): bool => project.name == "toolbox";
    let maybeToolbox =
      parsed.projects
      ->Belt.Option.flatMap(projects =>
          projects->Belt.List.keep(isToolbox)->Belt.List.head
        );
    let my_assert =
      switch (maybeToolbox) {
      | None => false
      | _ => true
      };
    expect(my_assert) |> toBe(true);
  });

  test("parse complete resources object", () => {
    expect(
      "sftests.json"->read->parse(Resources.decode).projects
      ->Belt.Option.getWithDefault([])
      ->Belt.List.length,
    )
    |> toBe(1)
  });

  test("parse resources repos", () =>
    expect(Belt.List.length(parsed.repos->Belt.Option.getExn)) |> toBe(1)
  );

  test("resources encoding roundtrip", () =>
    parsed->Resources.encode->Js.Json.stringifyWithSpace(2)->expect
    |> toEqual(raw)
  );

  test("v2 fromV1 encoding", () =>
    "ansible.json"
    ->read
    ->parse(Resources.decode)
    ->V2.fromV1(~defaultTenant="local")
    ->V2.t_encode
    ->Js.Json.stringifyWithSpace(2)
    ->expect
    |> toEqual("ansible-v2.json"->read->Js.String.trim)
  );

  test("parse group object", () => {
    let json = {|
        {
          "description": "The DLRN core group",
          "name": "dlrn-core",
          "members": ["member1", "member2"]
        }
      |};
    let expected =
      Group.{
        name: "dlrn-core",
        description: Some("The DLRN core group"),
        members: ["member1", "member2"],
      };
    let group = json->parse(Group.decode);
    expect(group.name) |> toBe(expected.name);
  });

  test("parse info", () => {
    expect("info.json"->read->parse(Info.decode).version) |> toBe("master")
  });
});
